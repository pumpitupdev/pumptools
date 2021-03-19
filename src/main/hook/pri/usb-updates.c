#define LOG_MODULE "prihook-usb-updates"

#include <string.h>

#include "capnhook/hook/filehook.h"

#include "util/log.h"

static enum cnh_result
prihook_usb_updates_filehook(struct cnh_filehook_irp *irp)
{
  if (irp->op != CNH_FILEHOOK_IRP_OP_OPEN) {
    return cnh_filehook_invoke_next(irp);
  }

  if (!strcmp(irp->open_filename, "/mnt/0/prime_") ||
      !strcmp(irp->open_filename, "/mnt/1/prime_")) {
    log_info("Blocking usb update %s", irp->open_filename);

    return CNH_RESULT_NO_SUCH_FILE_OR_DIR;
  }

  return cnh_filehook_invoke_next(irp);
}

void prihook_usb_updates_init(void)
{
  cnh_filehook_push_handler(prihook_usb_updates_filehook);
}