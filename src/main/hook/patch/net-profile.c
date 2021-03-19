#define LOG_MODULE "patch-net-profile"

#include <pthread.h>
#include <string.h>

#include "asset/nx2/lib/usb-rank.h"
#include "asset/nx2/lib/usb-save.h"

#include "capnhook/hook/filehook.h"

#include "pumpnet/lib/profile-token.h"
#include "pumpnet/lib/pumpnet.h"

#include "util/fs.h"
#include "util/log.h"
#include "util/time.h"

#define PUMPNET_MAX_NUM_PLAYERS 2
#define PUMPNET_PROFILE_FILE_NAME "pumpnet.bin"
#define PUMPNET_TRY_CONNECT_TIMEOUT_MS 2000

struct profile_virtual_mnt_point_file_info {
  int player;
  enum pumpnet_lib_file_type file_type;
  const char *file_path;
  size_t file_size;
};

struct profile_virtual_mnt_point_info {
  struct {
    struct profile_virtual_mnt_point_file_info
        file_info[PUMPNET_LIB_FILE_TYPE_COUNT];
  } player[PUMPNET_MAX_NUM_PLAYERS];
};

struct profile_virtual_file {
  uint64_t player_ref_id;
  FILE *handle;
  const struct profile_virtual_mnt_point_file_info *file_info;
  size_t buffer_pos;
  uint8_t *buffer;
  bool is_written;
};

struct profile_virtual_mnt_point {
  struct profile_virtual_file files[PUMPNET_LIB_FILE_TYPE_COUNT];
};

static enum cnh_result
_patch_net_profile_filehook(struct cnh_filehook_irp *irp);
static enum cnh_result _patch_net_profile_fopen(struct cnh_filehook_irp *irp);
static enum cnh_result _patch_net_profile_fclose(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file);
static enum cnh_result _patch_net_profile_fread(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file);
static enum cnh_result _patch_net_profile_fwrite(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file);
static enum cnh_result _patch_net_profile_fseek(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file);
static enum cnh_result _patch_net_profile_ftell(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file);
static enum cnh_result _patch_net_profile_feof(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file);

static struct profile_virtual_file *
_patch_net_profile_get_virtual_mnt_point(FILE *handle);
static bool _patch_net_profile_determine_is_profile_and_player_and_file_type(
    struct cnh_filehook_irp *irp,
    uint8_t *player,
    enum pumpnet_lib_file_type *file_type);
static enum cnh_result _patch_net_profile_open_pumpnet_profile_file(
    struct cnh_filehook_irp *irp,
    uint8_t player,
    enum pumpnet_lib_file_type file_type);
static bool _patch_net_profile_close_pumpnet_profile_file(
    struct profile_virtual_file *virtual_file);

static const struct profile_virtual_mnt_point_info
    _patch_net_profile_virtual_mnt_point_infos[] = {
        {.player = {
             {.file_info =
                  {{.player = 0,
                    .file_type = PUMPNET_LIB_FILE_TYPE_SAVE,
                    .file_path = "/mnt/0/nx2save.bin",
                    .file_size = ASSET_NX2_USB_SAVE_SIZE},
                   {.player = 0,
                    .file_type = PUMPNET_LIB_FILE_TYPE_RANK,
                    .file_path = "/mnt/0/nx2rank.bin",
                    .file_size = ASSET_NX2_USB_RANK_SIZE}}},
             {.file_info =
                  {{.player = 1,
                    .file_type = PUMPNET_LIB_FILE_TYPE_SAVE,
                    .file_path = "/mnt/1/nx2save.bin",
                    .file_size = ASSET_NX2_USB_SAVE_SIZE},
                   {.player = 1,
                    .file_type = PUMPNET_LIB_FILE_TYPE_RANK,
                    .file_path = "/mnt/1/nx2rank.bin",
                    .file_size = ASSET_NX2_USB_RANK_SIZE}}},
         }}};

static const struct profile_virtual_mnt_point_info
    *_patch_net_profile_file_info_ref;
static struct profile_virtual_mnt_point
    _patch_net_profile_virtual_mnt_points[PUMPNET_MAX_NUM_PLAYERS];
static pthread_mutex_t _patch_net_profile_mutex;

static enum cnh_result _patch_net_profile_filehook(struct cnh_filehook_irp *irp)
{
  if (irp->op == CNH_FILEHOOK_IRP_OP_OPEN) {
    return _patch_net_profile_fopen(irp);
  } else {
    struct profile_virtual_file *virt_file =
        _patch_net_profile_get_virtual_mnt_point(irp->file);

    if (virt_file != NULL) {
      switch (irp->op) {
        case CNH_FILEHOOK_IRP_OP_CLOSE:
          return _patch_net_profile_fclose(irp, virt_file);

        case CNH_FILEHOOK_IRP_OP_READ:
          return _patch_net_profile_fread(irp, virt_file);

        case CNH_FILEHOOK_IRP_OP_WRITE:
          return _patch_net_profile_fwrite(irp, virt_file);

        case CNH_FILEHOOK_IRP_OP_FGETS:
          log_die("Unsupported operation");
          return CNH_RESULT_OTHER_ERROR;

        case CNH_FILEHOOK_IRP_OP_SEEK:
          return _patch_net_profile_fseek(irp, virt_file);

        case CNH_FILEHOOK_IRP_OP_TELL:
          return _patch_net_profile_ftell(irp, virt_file);

        case CNH_FILEHOOK_IRP_OP_EOF:
          return _patch_net_profile_feof(irp, virt_file);

        default:
          log_die("Unhandled switch case");
          return CNH_RESULT_OTHER_ERROR;
      }
    } else {
      return cnh_filehook_invoke_next(irp);
    }
  }
}

static enum cnh_result _patch_net_profile_fopen(struct cnh_filehook_irp *irp)
{
  uint8_t player;
  enum pumpnet_lib_file_type file_type;
  player = -1;
  file_type = PUMPNET_LIB_FILE_TYPE_COUNT;

  // not a profile file, pass
  if (!_patch_net_profile_determine_is_profile_and_player_and_file_type(
          irp, &player, &file_type)) {
    return cnh_filehook_invoke_next(irp);
  }

  return _patch_net_profile_open_pumpnet_profile_file(irp, player, file_type);
}

static enum cnh_result _patch_net_profile_fclose(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file)
{
  log_debug("fclose %s", virtual_file->file_info->file_path);

  return _patch_net_profile_close_pumpnet_profile_file(virtual_file) ?
      CNH_RESULT_SUCCESS :
      CNH_RESULT_OTHER_ERROR;
}

static enum cnh_result _patch_net_profile_fread(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file)
{
  uint32_t len;

  pthread_mutex_lock(&_patch_net_profile_mutex);

  log_debug(
      "fread %s, pos 0x%X, bytes %d",
      virtual_file->file_info->file_path,
      virtual_file->buffer_pos,
      irp->read.nbytes);

  /* read up to eof, only */
  if (virtual_file->buffer_pos + irp->read.nbytes <
      virtual_file->file_info->file_size) {
    len = irp->read.nbytes;
  } else {
    len = virtual_file->file_info->file_size - virtual_file->buffer_pos;
  }

  memcpy(
      &irp->read.bytes[irp->read.pos],
      virtual_file->buffer + virtual_file->buffer_pos,
      len);

  virtual_file->buffer_pos += len;
  irp->read.pos += len;

  pthread_mutex_unlock(&_patch_net_profile_mutex);

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_net_profile_fwrite(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file)
{
  uint32_t len;

  pthread_mutex_lock(&_patch_net_profile_mutex);

  log_debug(
      "fwrite %s, pos 0x%X, bytes %d",
      virtual_file->file_info->file_path,
      virtual_file->buffer_pos,
      irp->read.nbytes);

  /* write up to eof, only */
  if (virtual_file->buffer_pos + irp->write.nbytes <
      virtual_file->file_info->file_size) {
    len = irp->write.nbytes;
  } else {
    len = virtual_file->file_info->file_size - virtual_file->buffer_pos;
  }

  memcpy(
      virtual_file->buffer + virtual_file->buffer_pos, irp->write.bytes, len);

  virtual_file->buffer_pos += len;
  virtual_file->is_written = true;
  irp->write.pos += len;

  pthread_mutex_unlock(&_patch_net_profile_mutex);

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_net_profile_fseek(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file)
{
  pthread_mutex_lock(&_patch_net_profile_mutex);

  switch (irp->seek_origin) {
    case SEEK_SET:
      virtual_file->buffer_pos = (size_t) irp->seek_offset;
      break;

    case SEEK_CUR:
      virtual_file->buffer_pos += (size_t) irp->seek_offset;
      break;

    case SEEK_END:
      virtual_file->buffer_pos = virtual_file->file_info->file_size;
      virtual_file->buffer_pos += (size_t) irp->seek_offset;
      break;

    default:
      log_die("Unhandled origin %d", irp->seek_origin);
      break;
  }

  log_debug(
      "fseek %s, offset 0x%X, origin %d",
      virtual_file->file_info->file_path,
      irp->seek_offset,
      irp->seek_origin);

  pthread_mutex_unlock(&_patch_net_profile_mutex);

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_net_profile_ftell(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file)
{
  pthread_mutex_lock(&_patch_net_profile_mutex);

  irp->tell_offset = virtual_file->buffer_pos;

  pthread_mutex_unlock(&_patch_net_profile_mutex);

  return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_net_profile_feof(
    struct cnh_filehook_irp *irp, struct profile_virtual_file *virtual_file)
{
  pthread_mutex_lock(&_patch_net_profile_mutex);

  irp->eof = virtual_file->buffer_pos == virtual_file->file_info->file_size;

  pthread_mutex_unlock(&_patch_net_profile_mutex);

  return CNH_RESULT_SUCCESS;
}

// =====================================================================================================================

static struct profile_virtual_file *
_patch_net_profile_get_virtual_mnt_point(FILE *handle)
{
  for (int i = 0; i < PUMPNET_MAX_NUM_PLAYERS; i++) {
    for (int j = 0; j < PUMPNET_LIB_FILE_TYPE_COUNT; j++) {
      if (_patch_net_profile_virtual_mnt_points->files[j].handle == handle) {
        return &_patch_net_profile_virtual_mnt_points->files[j];
      }
    }
  }

  return NULL;
}

static bool _patch_net_profile_determine_is_profile_and_player_and_file_type(
    struct cnh_filehook_irp *irp,
    uint8_t *player,
    enum pumpnet_lib_file_type *file_type)
{
  log_assert(irp);
  log_assert(player);
  log_assert(file_type);

  // compare to static string to avoid slowing everything down
  for (int i = 0; i < PUMPNET_MAX_NUM_PLAYERS; i++) {
    for (int j = 0; j < PUMPNET_LIB_FILE_TYPE_COUNT; j++) {
      if (!strcmp(
              irp->open_filename,
              _patch_net_profile_virtual_mnt_point_infos->player[i]
                  .file_info[j]
                  .file_path)) {
        *player = i;
        *file_type = (enum pumpnet_lib_file_type) j;
        return true;
      }
    }
  }

  return false;
}

static struct profile_virtual_file *_patch_net_profile_setup_virtual_file(
    uint8_t player,
    enum pumpnet_lib_file_type file_type,
    uint64_t player_ref_id)
{
  struct profile_virtual_file *virtual_file;

  log_debug("Setup virtual file, player %d, file_type %d", player, file_type);

  virtual_file =
      &_patch_net_profile_virtual_mnt_points[player].files[file_type];

  // sanity checks
  log_assert(!virtual_file->handle);
  log_assert(!virtual_file->file_info);
  log_assert(!virtual_file->buffer);

  virtual_file->player_ref_id = player_ref_id;
  virtual_file->handle = cnh_filehook_open_dummy_file_handle();
  virtual_file->file_info =
      &_patch_net_profile_virtual_mnt_point_infos->player[player]
           .file_info[file_type];
  virtual_file->buffer_pos = 0;
  virtual_file->is_written = false;

  virtual_file->buffer = (uint8_t *) malloc(virtual_file->file_info->file_size);

  log_debug(
      "Setup virtual file done, size %d", virtual_file->file_info->file_size);

  return virtual_file;
}

static void _patch_net_profile_destroy_virtual_file(
    uint8_t player, enum pumpnet_lib_file_type file_type)
{
  struct profile_virtual_file *virtual_file;

  log_debug(
      "Destroying virtual file, player %d, file_type %d", player, file_type);

  virtual_file =
      &_patch_net_profile_virtual_mnt_points[player].files[file_type];

  // sanity checks
  log_assert(virtual_file->handle);
  log_assert(virtual_file->file_info);
  log_assert(virtual_file->buffer);

  cnh_filehook_close_dummy_file_handle(virtual_file->handle);
  virtual_file->handle = NULL;
  virtual_file->file_info = NULL;
  free(virtual_file->buffer);
  virtual_file->buffer = NULL;
}

static bool _patch_net_profile_get_token(int player, uint64_t *player_ref_id)
{
  log_assert(player_ref_id);
  log_assert(player >= 0);
  log_assert(player <= 1);

  char path_profile_token[128];

  *player_ref_id = 0;

  sprintf(path_profile_token, "/mnt/%d/%s", player, PUMPNET_PROFILE_FILE_NAME);

  if (util_fs_path_exists(path_profile_token)) {
    log_info("Found pumpnet profile file %s", path_profile_token);

    if (pumpnet_lib_profile_token_load(path_profile_token, player_ref_id)) {
      return true;
    } else {
      log_error(
          "Loading pumpnet profile token %s failed, fallback to local profiles",
          path_profile_token);
    }
  } else {
    log_info("No pumpnet profile found, fallback to local profile");
  }

  return false;
}

static bool _patch_net_profile_download_profile_file(
    struct profile_virtual_file *virtual_file)
{
  int player;
  enum pumpnet_lib_file_type file_type;
  uint64_t player_ref_id;

  player = virtual_file->file_info->player;
  file_type = virtual_file->file_info->file_type;
  player_ref_id = virtual_file->player_ref_id;

  log_info(
      "Profile file player %d, file_type %d, refId %llX downloading from "
      "server...",
      player,
      file_type,
      player_ref_id);

  if (!pumpnet_lib_get(
          file_type,
          player_ref_id,
          virtual_file->buffer,
          virtual_file->file_info->file_size)) {
    log_error(
        "Downloading file player %d, file_type %d, refId %llX failed",
        player,
        file_type,
        player_ref_id);
    return false;
  }

  log_info(
      "Downloading file player %d, file_type %d, refId %llX successful",
      player,
      file_type,
      player_ref_id);
  return true;
}

static enum cnh_result _patch_net_profile_open_pumpnet_profile_file(
    struct cnh_filehook_irp *irp,
    uint8_t player,
    enum pumpnet_lib_file_type file_type)
{
  uint64_t player_ref_id;

  log_debug("open %s %d %d", irp->open_filename, player, file_type);

  // check if pumpnet.bin token is available and server reachable
  // download all profile files on the first file request and hook all following
  // files to virtual endpoints of the already downloaded and buffered data
  if (_patch_net_profile_get_token(player, &player_ref_id)) {
    struct profile_virtual_file *virtual_file =
        _patch_net_profile_setup_virtual_file(player, file_type, player_ref_id);

    if (_patch_net_profile_download_profile_file(virtual_file)) {
      irp->file = virtual_file->handle;
      pthread_mutex_unlock(&_patch_net_profile_mutex);

      return CNH_RESULT_SUCCESS;
    } else {
      _patch_net_profile_destroy_virtual_file(player, file_type);

      pthread_mutex_unlock(&_patch_net_profile_mutex);

      return CNH_RESULT_NO_SUCH_FILE_OR_DIR;
    }
  }

  return cnh_filehook_invoke_next(irp);
}

static bool _patch_net_profile_upload_profile_file(
    struct profile_virtual_file *virtual_file)
{
  int player;
  enum pumpnet_lib_file_type file_type;
  uint64_t player_ref_id;

  player = virtual_file->file_info->player;
  file_type = virtual_file->file_info->file_type;
  player_ref_id = virtual_file->player_ref_id;

  log_info(
      "Profile file player %d, file_type %d, refId %llX uploading to server...",
      player,
      file_type,
      player_ref_id);

  if (!pumpnet_lib_put(
          file_type,
          player_ref_id,
          virtual_file->buffer,
          virtual_file->file_info->file_size)) {
    log_error(
        "Uploading file player %d, file_type %d, refId %llX failed",
        player,
        file_type,
        player_ref_id);
    return false;
  }

  log_info(
      "Uploading file player %d, file_type %d, refId %llX successful",
      player,
      file_type,
      player_ref_id);
  return true;
}

static bool _patch_net_profile_close_pumpnet_profile_file(
    struct profile_virtual_file *virtual_file)
{
  bool res = true;

  pthread_mutex_lock(&_patch_net_profile_mutex);

  if (virtual_file->is_written) {
    res = _patch_net_profile_upload_profile_file(virtual_file);
  }

  _patch_net_profile_destroy_virtual_file(
      virtual_file->file_info->player, virtual_file->file_info->file_type);

  pthread_mutex_unlock(&_patch_net_profile_mutex);

  return res;
}

// =====================================================================================================================

void patch_net_profile_init(
    enum asset_game_version game,
    const char *pumpnet_server_addr,
    uint64_t machine_id,
    const char *cert_dir_path,
    bool verbose_debug_log)
{
  uint8_t idx;

  /* use a switch instead of offset'ing the enum for additional safety and block
   * unsupported games */
  switch (game) {
    case ASSET_GAME_VERSION_NX2:
      idx = 0;
      break;

      //        case PUMPNET_LIB_GAME_NXA:
      //            idx = 1;
      //            break;
      //
      //        case PUMPNET_LIB_GAME_FST:
      //            idx = 2;
      //            break;
      //
      //        case PUMPNET_LIB_GAME_FEX:
      //            idx = 3;
      //            break;
      //
      //        case PUMPNET_LIB_GAME_F2:
      //            idx = 4;
      //            break;
    case ASSET_GAME_VERSION_NXA:
    case ASSET_GAME_VERSION_FIESTA:
    case ASSET_GAME_VERSION_FIESTA_EX:
    case ASSET_GAME_VERSION_FIESTA_2:

    default:
      log_error(
          "Unsupported game version %d specified, disabled network service",
          game);
      return;
  }

  _patch_net_profile_file_info_ref =
      &_patch_net_profile_virtual_mnt_point_infos[idx];

  size_t max_data_size = 0;

  // determine max file size for network buffers, file sizes identical on for
  // all players.
  for (int i = 0; i < PUMPNET_LIB_FILE_TYPE_COUNT; i++) {
    max_data_size +=
        _patch_net_profile_file_info_ref->player[0].file_info[i].file_size;
  }

  // set buffers to NULL to detect illegal states
  for (int i = 0; i < PUMPNET_LIB_FILE_TYPE_COUNT; i++) {
    for (int j = 0; j < PUMPNET_MAX_NUM_PLAYERS; j++) {
      _patch_net_profile_virtual_mnt_points[j].files[i].player_ref_id = 0;
      _patch_net_profile_virtual_mnt_points[j].files[i].handle = NULL;
      _patch_net_profile_virtual_mnt_points[j].files[i].file_info = NULL;
      _patch_net_profile_virtual_mnt_points[j].files[i].buffer = NULL;
    }
  }

  pumpnet_lib_init(
      game, pumpnet_server_addr, machine_id, cert_dir_path, verbose_debug_log);

  cnh_filehook_push_handler(_patch_net_profile_filehook);

  pthread_mutex_init(&_patch_net_profile_mutex, NULL);

  log_info(
      "Initialized: game %d, server %s, machine id %llX",
      game,
      pumpnet_server_addr,
      machine_id);
}

void patch_net_profile_shutdown()
{
  pumpnet_lib_shutdown();

  pthread_mutex_destroy(&_patch_net_profile_mutex);

  log_info("Shut down");
}