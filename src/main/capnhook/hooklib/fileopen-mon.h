/**
 * Hook implementation to monitor different kinds of operations opening a file
 */
#ifndef CAPNHOOK_FILEOPEN_MON_H
#define CAPNHOOK_FILEOPEN_MON_H

#include "capnhook/hook/filehook.h"
#include "capnhook/hook/fshook.h"
#include "capnhook/hook/iohook.h"

/**
 * Hook function to add to the hook module for monitoring open file calls on file operations
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_fileopen_mon_filehook(struct cnh_filehook_irp *irp);

/**
 * Hook function to add to the hook module for monitoring open file calls on file system related operations
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_fileopen_mon_fshook(struct cnh_fshook_irp *irp);

/**
 * Hook function to add to the hook module for monitoring open file/device calls on I/O operations
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_fileopen_mon_iohook(struct cnh_iohook_irp *irp);

#endif
