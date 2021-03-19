/**
 * Hook implementation to redirect files/paths passed as parameters on various
 * file, file system and I/O calls
 */
#ifndef CAPNHOOK_REDIR_H
#define CAPNHOOK_REDIR_H

#include "capnhook/hook/filehook.h"
#include "capnhook/hook/fshook.h"
#include "capnhook/hook/iohook.h"

/**
 * Add a new path/file redirect to the module
 *
 * @param src Source path/file to redirect. This does not have to match the full
 * path. Postfix, e.g. just the filename is sufficient as well
 * @param dest Destination to redirect the original path to. Recommended to use
 * absolute paths here.
 */
void cnh_redir_add(const char *src, const char *dest);

/**
 * Hook function to add to the hook module for redirecting file system related
 * calls
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_redir_fshook(struct cnh_fshook_irp *irp);

/**
 * Hook function to add to the hook module for redirecting I/O calls
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_redir_iohook(struct cnh_iohook_irp *irp);

/**
 * Hook function to add to the hook module for redirecting file related calls
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_redir_filehook(struct cnh_filehook_irp *irp);

#endif