#define LOG_MODULE "cnh-iohook-mon"

#include "capnhook/hooklib/iohook-mon.h"

#include "util/log.h"

static const char *_cnh_iohook_mon_str_empty = "";
static const char *_cnh_iohook_mon_irp_op_str[] = {
    "open", "fdopen", "close", "read", "write", "seek", "ioctl"};

enum cnh_result cnh_iohook_mon(struct cnh_iohook_irp *irp)
{
  enum cnh_result result;

  log_debug(
      "[before][%s] next_handler %d, fd %d, open_filename %s, open_flags %d, "
      "fdopen_fd %d, fdopen_mode %s, "
      "fdopen_res %p, read(bytes %p, nbytes %d, pos %d), write(bytes %p, "
      "nbytes %d, pos %d), seek_origin %d, "
      "seek_offset %d, seek_pos %llu, ioctl_req %d, ioctl(bytes %p, nbytes %d, "
      "pos %d)",
      _cnh_iohook_mon_irp_op_str[irp->op],
      irp->next_handler,
      irp->fd,
      irp->open_filename ? irp->open_filename : _cnh_iohook_mon_str_empty,
      irp->open_flags,
      irp->fdopen_fd,
      irp->fdopen_mode ? irp->fdopen_mode : _cnh_iohook_mon_str_empty,
      irp->fdopen_res,
      irp->read.bytes,
      irp->read.nbytes,
      irp->read.pos,
      irp->write.bytes,
      irp->write.nbytes,
      irp->write.pos,
      irp->seek_origin,
      irp->seek_offset,
      irp->seek_pos,
      irp->ioctl_req,
      irp->ioctl.bytes,
      irp->ioctl.nbytes,
      irp->ioctl.pos);

  result = cnh_iohook_invoke_next(irp);

  if (result != CNH_RESULT_SUCCESS) {
    log_error(
        "[after][%s] next_handler %d, res: %d, fd %d, open_filename %s, "
        "open_flags %d, fdopen_fd %d, "
        "fdopen_mode %s, fdopen_res %p, read(bytes %p, nbytes %d, pos %d), "
        "write(bytes %p, nbytes %d, pos %d), "
        "seek_origin %d, seek_offset %d, seek_pos %llu, ioctl_req %d, "
        "ioctl(bytes %p, nbytes %d, pos %d)",
        _cnh_iohook_mon_irp_op_str[irp->op],
        irp->next_handler,
        result,
        irp->fd,
        irp->open_filename ? irp->open_filename : _cnh_iohook_mon_str_empty,
        irp->open_flags,
        irp->fdopen_fd,
        irp->fdopen_mode ? irp->fdopen_mode : _cnh_iohook_mon_str_empty,
        irp->fdopen_res,
        irp->read.bytes,
        irp->read.nbytes,
        irp->read.pos,
        irp->write.bytes,
        irp->write.nbytes,
        irp->write.pos,
        irp->seek_origin,
        irp->seek_offset,
        irp->seek_pos,
        irp->ioctl_req,
        irp->ioctl.bytes,
        irp->ioctl.nbytes,
        irp->ioctl.pos);
  } else {
    log_debug(
        "[after][%s] next_handler %d, res %d, fd %d, open_filename %s, "
        "open_flags %d, fdopen_fd %d, "
        "fdopen_mode %s, fdopen_res %p, read(bytes %p, nbytes %d, pos %d), "
        "write(bytes %p, nbytes %d, pos %d), "
        "seek_origin %d, seek_offset %d, seek_pos %llu, ioctl_req %d, "
        "ioctl(bytes %p, nbytes %d, pos %d)",
        _cnh_iohook_mon_irp_op_str[irp->op],
        irp->next_handler,
        result,
        irp->fd,
        irp->open_filename ? irp->open_filename : _cnh_iohook_mon_str_empty,
        irp->open_flags,
        irp->fdopen_fd,
        irp->fdopen_mode ? irp->fdopen_mode : _cnh_iohook_mon_str_empty,
        irp->fdopen_res,
        irp->read.bytes,
        irp->read.nbytes,
        irp->read.pos,
        irp->write.bytes,
        irp->write.nbytes,
        irp->write.pos,
        irp->seek_origin,
        irp->seek_offset,
        irp->seek_pos,
        irp->ioctl_req,
        irp->ioctl.bytes,
        irp->ioctl.nbytes,
        irp->ioctl.pos);
  }

  return result;
}