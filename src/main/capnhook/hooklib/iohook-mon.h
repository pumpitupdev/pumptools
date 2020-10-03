/**
 * Hook implementation to monitor I/O calls, e.g. open, read, write etc.
 */
#ifndef CAPNHOOK_IOHOOK_MON_H
#define CAPNHOOK_IOHOOK_MON_H

#include "capnhook/hook/iohook.h"

/**
 * Hook function to add to the hook module for monitoring I/O calls
 *
 * @param irp I/O request package of hook module received
 * @return Result of operation
 */
enum cnh_result cnh_iohook_mon(struct cnh_iohook_irp *irp);

#endif