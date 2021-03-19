#include "capnhook/hook/filehook.h"
#include "capnhook/hook/fshook.h"
#include "capnhook/hook/iohook.h"

#include "util/log.h"

enum cnh_result cnh_fileopen_mon_filehook(struct cnh_filehook_irp *irp)
{
  enum cnh_result result;

  if (irp->op == CNH_FILEHOOK_IRP_OP_OPEN) {
    log_debug("[before][fopen]: %s %s", irp->open_filename, irp->open_mode);

    result = cnh_filehook_invoke_next(irp);

    log_debug(
        "[after][fopen]: %s %s -> %d (%d)",
        irp->open_filename,
        irp->open_mode,
        irp->file,
        result);
  } else {
    result = cnh_filehook_invoke_next(irp);
  }

  return result;
}

enum cnh_result cnh_fileopen_mon_fshook(struct cnh_fshook_irp *irp)
{
  enum cnh_result result;

  switch (irp->op) {
    case CNH_FSHOOK_IRP_OP_DIR_OPEN:
      log_debug("[before][diropen]: %s", irp->opendir_name);
      break;
    case CNH_FSHOOK_IRP_OP_LXSTAT:
      log_debug("[before][lxstat]: %s", irp->xstat_file);
      break;
    case CNH_FSHOOK_IRP_OP_XSTAT:
      log_debug("[before][xstat]: %s", irp->xstat_file);
      break;
    case CNH_FSHOOK_IRP_OP_RENAME:
      log_debug("[before][rename]: %s %s", irp->rename_old, irp->rename_new);
      break;
    case CNH_FSHOOK_IRP_OP_REMOVE:
      log_debug("[before][remove]: %s", irp->remove_pathname);
      break;
    case CNH_FSHOOK_IRP_OP_ACCESS:
      log_debug("[before][access]: %s", irp->access_path);
      break;
    default:
      log_die("Unhandled case: %d", irp->op);
      break;
  }

  result = cnh_fshook_invoke_next(irp);

  switch (irp->op) {
    case CNH_FSHOOK_IRP_OP_DIR_OPEN:
      log_debug("[after][diropen]: %s -> %d", irp->opendir_name, result);
      break;
    case CNH_FSHOOK_IRP_OP_LXSTAT:
      log_debug("[after][lxstat]: %s -> %d", irp->xstat_file, result);
      break;
    case CNH_FSHOOK_IRP_OP_XSTAT:
      log_debug("[after][xstat]: %s -> %d", irp->xstat_file, result);
      break;
    case CNH_FSHOOK_IRP_OP_RENAME:
      log_debug(
          "[after][rename]: %s %s -> %d",
          irp->rename_old,
          irp->rename_new,
          result);
      break;
    case CNH_FSHOOK_IRP_OP_REMOVE:
      log_debug("[after][remove]: %s -> %d", irp->remove_pathname, result);
      break;
    case CNH_FSHOOK_IRP_OP_ACCESS:
      log_debug("[after][access]: %s -> %d", irp->access_path, result);
      break;
    default:
      log_die("Unhandled case: %d", irp->op);
      break;
  }

  return result;
}

enum cnh_result cnh_fileopen_mon_iohook(struct cnh_iohook_irp *irp)
{
  enum cnh_result result;

  if (irp->op == CNH_IOHOOK_IRP_OP_OPEN) {
    log_debug("[before][open]: %s %d", irp->open_filename, irp->open_flags);

    result = cnh_iohook_invoke_next(irp);

    log_debug(
        "[after][open]: %s %d -> %d (%d)",
        irp->open_filename,
        irp->open_flags,
        irp->fd,
        result);
  } else {
    result = cnh_iohook_invoke_next(irp);
  }

  return result;
}