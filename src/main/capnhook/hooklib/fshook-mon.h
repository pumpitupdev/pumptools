/**
 * Hook implementation to monitor file system related calls
 */
#ifndef CAPNHOOK_FSHOOK_MON_H
#define CAPNHOOK_FSHOOK_MON_H

#include "capnhook/hook/fshook.h"

/**
 * Hook function to add to the hook module for monitoring file system related calls
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_fshook_mon(struct cnh_fshook_irp *irp);

#endif