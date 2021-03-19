#define LOG_MODULE "cnh-fshook-mon"

#include "capnhook/hooklib/fshook-mon.h"

#include "util/log.h"

static const char *_cnh_fshook_mon_irp_op_str[] = {
    "diropen", "lxstat", "xstat", "rename", "remove", "access"};

enum cnh_result cnh_fshook_mon(struct cnh_fshook_irp *irp)
{
  enum cnh_result result;

  log_debug(
      "[before][%s] next_handler %d, opendir_name %s, opendir_ret %p, "
      "xstat_version %d, xstat_file %s, "
      "xstat_buf %p, rename_old %s, rename_new %s, remove_pathname %s, "
      "access_path %s, access_amode %d",
      _cnh_fshook_mon_irp_op_str[irp->op],
      irp->next_handler,
      irp->opendir_name,
      irp->opendir_ret,
      irp->xstat_version,
      irp->xstat_file,
      irp->xstat_buf,
      irp->rename_old,
      irp->rename_new,
      irp->remove_pathname,
      irp->access_path,
      irp->access_amode);

  result = cnh_fshook_invoke_next(irp);

  if (result != CNH_RESULT_SUCCESS) {
    log_error(
        "[before][%s] result %d, next_handler %d, opendir_name %s, opendir_ret "
        "%p, xstat_version %d, "
        "xstat_file %s, xstat_buf %p, rename_old %s, rename_new %s, "
        "remove_pathname %s, access_path %s, "
        "access_amode %d",
        _cnh_fshook_mon_irp_op_str[irp->op],
        result,
        irp->next_handler,
        irp->opendir_name,
        irp->opendir_ret,
        irp->xstat_version,
        irp->xstat_file,
        irp->xstat_buf,
        irp->rename_old,
        irp->rename_new,
        irp->remove_pathname,
        irp->access_path,
        irp->access_amode);
  } else {
    log_debug(
        "[before][%s] result %d, next_handler %d, opendir_name %s, opendir_ret "
        "%p, xstat_version %d, "
        "xstat_file %s, xstat_buf %p, rename_old %s, rename_new %s, "
        "remove_pathname %s, access_path %s, "
        "access_amode %d",
        _cnh_fshook_mon_irp_op_str[irp->op],
        result,
        irp->next_handler,
        irp->opendir_name,
        irp->opendir_ret,
        irp->xstat_version,
        irp->xstat_file,
        irp->xstat_buf,
        irp->rename_old,
        irp->rename_new,
        irp->remove_pathname,
        irp->access_path,
        irp->access_amode);
  }

  return result;
}