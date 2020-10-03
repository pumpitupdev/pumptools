/**
 * Hook implementation to monitor file related calls, e.g. fopen, fread, fwrite etc.
 */
#ifndef CAPNHOOK_FILEHOOK_MON_H
#define CAPNHOOK_FILEHOOK_MON_H

#include "capnhook/hook/filehook.h"

/**
 * Hook function to add to the hook module for monitoring file related calls
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_filehook_mon(struct cnh_filehook_irp *irp);

#endif
