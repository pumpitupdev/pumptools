#define LOG_MODULE "cnh-filehook-mon"

#include "capnhook/hooklib/filehook-mon.h"

#include "util/log.h"

static const char *_cnh_filehook_mon_str_empty = "";
static const char *_cnh_filehook_mon_irp_op_str[] = {
    "fopen", "fclose", "fread", "fwrite", "fgets", "fseek", "ftell", "feof"};

enum cnh_result cnh_filehook_mon(struct cnh_filehook_irp *irp)
{
  enum cnh_result result;

  /* Skip writes to log file because this results in infinite recursion */
  if (irp->file == util_log_get_file_handle()) {
    return cnh_filehook_invoke_next(irp);
  }

  log_debug(
      "[before][%s] next_handler %d, file %p, open_filename %s, open_mode %s, "
      "read(bytes %p, nbytes %d, pos %d)"
      ", write(bytes %p, nbytes %d, pos %d), orig_read_write_size %d, "
      "orig_read_write_nmemb %d, seek_origin %d, "
      "seek_offset %d, tell_offset %llu, eof %d",
      _cnh_filehook_mon_irp_op_str[irp->op],
      irp->next_handler,
      irp->file,
      irp->open_filename ? irp->open_filename : _cnh_filehook_mon_str_empty,
      irp->open_mode ? irp->open_mode : _cnh_filehook_mon_str_empty,
      irp->read.bytes,
      irp->read.nbytes,
      irp->read.pos,
      irp->write.bytes,
      irp->write.nbytes,
      irp->write.pos,
      irp->orig_read_write_size,
      irp->orig_read_write_nmemb,
      irp->seek_origin,
      irp->seek_offset,
      irp->tell_offset,
      irp->eof);

  result = cnh_filehook_invoke_next(irp);

  if (result != CNH_RESULT_SUCCESS) {
    log_error(
        "[after][%s] result %d, next_handler %d, file %p, open_filename %s, "
        "open_mode %s, read(bytes %p, "
        "nbytes %d, pos %d), write(bytes %p, nbytes %d, pos %d), "
        "orig_read_write_size %d, orig_read_write_nmemb %d,"
        " seek_origin %d, seek_offset %d, tell_offset %llu, eof %d",
        _cnh_filehook_mon_irp_op_str[irp->op],
        result,
        irp->next_handler,
        irp->file,
        irp->open_filename ? irp->open_filename : _cnh_filehook_mon_str_empty,
        irp->open_mode ? irp->open_mode : _cnh_filehook_mon_str_empty,
        irp->read.bytes,
        irp->read.nbytes,
        irp->read.pos,
        irp->write.bytes,
        irp->write.nbytes,
        irp->write.pos,
        irp->orig_read_write_size,
        irp->orig_read_write_nmemb,
        irp->seek_origin,
        irp->seek_offset,
        irp->tell_offset,
        irp->eof);
  } else {
    log_debug(
        "[after][%s] result %d, next_handler %d, file %p, open_filename %s, "
        "open_mode %s, read(bytes %p, "
        "nbytes %d, pos %d), write(bytes %p, nbytes %d, pos %d), "
        "orig_read_write_size %d, orig_read_write_nmemb %d,"
        " seek_origin %d, seek_offset %d, tell_offset %llu, eof %d",
        _cnh_filehook_mon_irp_op_str[irp->op],
        result,
        irp->next_handler,
        irp->file,
        irp->open_filename ? irp->open_filename : _cnh_filehook_mon_str_empty,
        irp->open_mode ? irp->open_mode : _cnh_filehook_mon_str_empty,
        irp->read.bytes,
        irp->read.nbytes,
        irp->read.pos,
        irp->write.bytes,
        irp->write.nbytes,
        irp->write.pos,
        irp->orig_read_write_size,
        irp->orig_read_write_nmemb,
        irp->seek_origin,
        irp->seek_offset,
        irp->tell_offset,
        irp->eof);
  }

  return result;
}