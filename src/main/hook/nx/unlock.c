#define LOG_MODULE "hook-nx-unlock"

#include <assert.h>
#include <string.h>

#include "asset/nx/lib/settings.h"

#include "capnhook/hook/filehook.h"

#include "util/log.h"
#include "util/str.h"

static FILE *_hook_nx_unlock_settings_file = NULL;

static enum cnh_result _hook_nx_unlock_filehook(struct cnh_filehook_irp *irp);

void hook_nx_unlock_init(void)
{
  cnh_filehook_push_handler(_hook_nx_unlock_filehook);

  log_info("Initialized");
}

static enum cnh_result _hook_nx_unlock_filehook(struct cnh_filehook_irp *irp)
{
  enum cnh_result result;

  assert(irp);

  if (irp->op != CNH_FILEHOOK_IRP_OP_OPEN &&
      irp->file != _hook_nx_unlock_settings_file) {
    return cnh_filehook_invoke_next(irp);
  }

  switch (irp->op) {
    case CNH_FILEHOOK_IRP_OP_OPEN: {
      /* Let it open the real file */
      result = cnh_filehook_invoke_next(irp);

      if (util_str_ends_with(irp->open_filename, ASSET_NX_SETTINGS_FILE_NAME)) {
        if (result == CNH_RESULT_SUCCESS) {
          log_debug("Intercepting settings file open");
          _hook_nx_unlock_settings_file = irp->file;
        }
      }

      break;
    }

    case CNH_FILEHOOK_IRP_OP_READ: {
      /* Let it read the real file */
      result = cnh_filehook_invoke_next(irp);

      if (result == CNH_RESULT_SUCCESS) {
        struct asset_nx_settings *settings =
            (struct asset_nx_settings *) irp->read.bytes;

        asset_nx_settings_unlock_all(settings);

        log_debug(
            "Applied unlock patch to loaded settings (buffer size %d)",
            irp->read.pos);
      }

      break;
    }

    case CNH_FILEHOOK_IRP_OP_CLOSE: {
      _hook_nx_unlock_settings_file = NULL;

      result = cnh_filehook_invoke_next(irp);

      break;
    }

    default:
      result = cnh_filehook_invoke_next(irp);
      break;
  }

  return result;
}