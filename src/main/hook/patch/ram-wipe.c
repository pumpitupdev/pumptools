#define LOG_MODULE "patch-ram-wipe"

#include <string.h>

#include "capnhook/hook/filehook.h"

#include "util/log.h"

static enum cnh_result _patch_ram_wipe_filehook(struct cnh_filehook_irp *irp);

void patch_ram_wipe_init(void)
{
  cnh_filehook_push_handler(_patch_ram_wipe_filehook);
  log_info("Initialized");
}

static enum cnh_result _patch_ram_wipe_filehook(struct cnh_filehook_irp *irp)
{
  if (irp->op == CNH_FILEHOOK_IRP_OP_OPEN &&
      !strcmp(irp->open_filename, "/dev/ram0")) {
    log_info("Killing ram wipe");
    return CNH_RESULT_NO_SUCH_FILE_OR_DIR;
  } else {
    return cnh_filehook_invoke_next(irp);
  }
}