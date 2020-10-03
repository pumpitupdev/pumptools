/**
 * Hook module for hooking I/O related calls, e.g. open, read, write etc.
 *
 * Based on original capnhook code for windows by decafcode:
 * https://github.com/decafcode/capnhook
 */
#ifndef CAPNHOOK_IOHOOK_H
#define CAPNHOOK_IOHOOK_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "capnhook/hook/iobuf.h"

/**
 * Available operations to hook
 */
enum cnh_iohook_irp_op {
    CNH_IOHOOK_IRP_OP_OPEN = 0,
    CNH_IOHOOK_IRP_OP_FDOPEN = 1,
    CNH_IOHOOK_IRP_OP_CLOSE = 2,
    CNH_IOHOOK_IRP_OP_READ = 3,
    CNH_IOHOOK_IRP_OP_WRITE = 4,
    CNH_IOHOOK_IRP_OP_SEEK = 5,
    CNH_IOHOOK_IRP_OP_IOCTL = 6,
};

/**
 * I/O request packet
 */
struct cnh_iohook_irp {
    enum cnh_iohook_irp_op op;
    size_t next_handler;
    int fd;
    const char *open_filename;
    int open_flags;
    int fdopen_fd;
    const char* fdopen_mode;
    FILE* fdopen_res;
    struct cnh_iobuf read;
    struct cnh_const_iobuf write;
    int seek_origin;
    int64_t seek_offset;
    uint64_t seek_pos;
    int ioctl_req;
    struct cnh_iobuf ioctl;
};

/**
 * Hook function type
 */
typedef enum cnh_result (*cnh_iohook_fn_t)(struct cnh_iohook_irp *irp);

/**
 * Add a new hook handler to the hook module.
 * The handler added is getting called on every hooked operation.
 *
 * @param fn Pointer to hook function to add
 * @return Result/error code if hooking was successful
 */
enum cnh_result cnh_iohook_push_handler(cnh_iohook_fn_t fn);

/**
 * Invoke the next hooked function registered in the hook module.
 * Call this from your hook handler if you want to pass on execution to further
 * hooked calls reaching the original function at the end of the hook chain.
 *
 * @param irp I/O request package to dispatch
 * @return Result of the following function dispatching the request package
 */
enum cnh_result cnh_iohook_invoke_next(struct cnh_iohook_irp *irp);

/**
 * Create a dummy file handle. Use this function for file access virtualization.
 * Do not execute real operations on the handle returned.
 *
 * @return Actual file handle instance pointing to a dummy endpoint.
 */
int cnh_iohook_open_dummy_fd();

/**
 * Close a dummy file handle. Make sure to free your previously allocated handles
 * to avoid resource leaks. Do not free them using the real close function.
 *
 * @param file Dummy file handle to free
 */
void cnh_iohook_close_dummy_fd(int fd);

#endif
