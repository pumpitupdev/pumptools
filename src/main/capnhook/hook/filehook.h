/**
 * Hook module for hooking file related calls, e.g. fopen, fread, fwrite, etc.
 *
 * Based on original capnhook code for windows by decafcode:
 * https://github.com/decafcode/capnhook
 */
#ifndef CAPNHOOK_FILEHOOK_H
#define CAPNHOOK_FILEHOOK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "capnhook/hook/iobuf.h"

/**
 * Available operations to hook
 */
enum cnh_filehook_irp_op {
    CNH_FILEHOOK_IRP_OP_OPEN = 0,
    CNH_FILEHOOK_IRP_OP_CLOSE = 1,
    CNH_FILEHOOK_IRP_OP_READ = 2,
    CNH_FILEHOOK_IRP_OP_WRITE = 3,
    CNH_FILEHOOK_IRP_OP_FGETS = 4,
    CNH_FILEHOOK_IRP_OP_SEEK = 5,
    CNH_FILEHOOK_IRP_OP_TELL = 6,
    CNH_FILEHOOK_IRP_OP_EOF = 7,
};

/**
 * I/O request packet
 */
struct cnh_filehook_irp {
    enum cnh_filehook_irp_op op;
    size_t next_handler;
    FILE* file;
    const char *open_filename;
    const char* open_mode;
    struct cnh_iobuf read;
    struct cnh_const_iobuf write;
    size_t orig_read_write_size;
    size_t orig_read_write_nmemb;
    int seek_origin;
    int64_t seek_offset;
    uint64_t tell_offset;
    bool eof;
};

/**
 * Hook function type
 */
typedef enum cnh_result (*cnh_filehook_fn_t)(struct cnh_filehook_irp *irp);

/**
 * Add a new hook handler to the hook module.
 * The handler added is getting called on every hooked operation.
 *
 * @param fn Pointer to hook function to add
 * @return Result/error code if hooking was successful
 */
enum cnh_result cnh_filehook_push_handler(cnh_filehook_fn_t fn);

/**
 * Invoke the next hooked function registered in the hook module.
 * Call this from your hook handler if you want to pass on execution to further
 * hooked calls reaching the original function at the end of the hook chain.
 *
 * @param irp I/O request package to dispatch
 * @return Result of the following function dispatching the request package
 */
enum cnh_result cnh_filehook_invoke_next(struct cnh_filehook_irp *irp);

/**
 * Create a dummy file handle. Use this function for file access virtualization.
 * Do not execute real operations on the handle returned.
 *
 * @return Actual file handle instance pointing to a dummy endpoint.
 */
FILE* cnh_filehook_open_dummy_file_handle();

/**
 * Close a dummy file handle. Make sure to free your previously allocated handles
 * to avoid resource leaks. Do not free them using the real close function.
 *
 * @param file Dummy file handle to free
 */
void cnh_filehook_close_dummy_file_handle(FILE* file);

#endif
