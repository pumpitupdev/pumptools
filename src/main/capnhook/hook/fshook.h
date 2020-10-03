/**
 * Hook module for hooking file system related calls,
 * not counting calls operating on actual opened files, e.g.
 * fopen, fread, fwrite etc.
 *
 * Based on original capnhook code for windows by decafcode:
 * https://github.com/decafcode/capnhook
 */
#ifndef CAPNHOOK_FSHOOK_H
#define CAPNHOOK_FSHOOK_H

#include <dirent.h>
#include <stddef.h>
#include <stdint.h>

#include "capnhook/hook/result.h"

/**
 * Available operations to hook
 */
enum cnh_fshook_irp_op {
    CNH_FSHOOK_IRP_OP_DIR_OPEN = 0,
    CNH_FSHOOK_IRP_OP_LXSTAT = 1,
    CNH_FSHOOK_IRP_OP_XSTAT = 2,
    CNH_FSHOOK_IRP_OP_RENAME = 3,
    CNH_FSHOOK_IRP_OP_REMOVE = 4,
    CNH_FSHOOK_IRP_OP_ACCESS = 5,
};

/**
 * I/O request packet
 */
struct cnh_fshook_irp {
    enum cnh_fshook_irp_op op;
    size_t next_handler;
    const char* opendir_name;
    DIR* opendir_ret;
    int xstat_version;
    const char* xstat_file;
    struct stat* xstat_buf;
    const char* rename_old;
    const char* rename_new;
    const char* remove_pathname;
    const char* access_path;
    int access_amode;
};

/**
 * Hook function type
 */
typedef enum cnh_result (*cnh_fshook_fn_t)(struct cnh_fshook_irp *irp);

/**
 * Add a new hook handler to the hook module.
 * The handler added is getting called on every hooked operation.
 *
 * @param fn Pointer to hook function to add
 * @return Result/error code if hooking was successful
 */
enum cnh_result cnh_fshook_push_handler(cnh_fshook_fn_t fn);

/**
 * Invoke the next hooked function registered in the hook module.
 * Call this from your hook handler if you want to pass on execution to further
 * hooked calls reaching the original function at the end of the hook chain.
 *
 * @param irp I/O request package to dispatch
 * @return Result of the following function dispatching the request package
 */
enum cnh_result cnh_fshook_invoke_next(struct cnh_fshook_irp *irp);

#endif
