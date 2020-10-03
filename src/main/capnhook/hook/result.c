#define LOG_MODULE "capnhook"

#include <errno.h>

#include "capnhook/hook/result.h"

#include "util/log.h"

int cnh_result_to_errno(enum cnh_result result)
{
    switch (result) {
        case CNH_RESULT_SUCCESS:
            return 0;
        case CNH_RESULT_ERROR_INSUFFICIENT_BUFFER:
            return EIO;
        case CNH_RESULT_INVALID_PARAMETER:
            return EINVAL;
        case CNH_RESULT_OUT_OF_MEMORY:
            return ENOMEM;
        case CNH_RESULT_REPEAT_OPERATION:
            return EAGAIN;
        case CNH_RESULT_NO_SUCH_FILE_OR_DIR:
            return ENOENT;
        case CNH_RESULT_NOT_TTY:
            return ENOTTY;
        case CNH_RESULT_BUSY:
            return EBUSY;
        case CNH_RESULT_IS_A_DIRECTORY:
            return EISDIR;
        case CNH_RESULT_PERMISSION_DENIED:
            return EACCES;
        case CNH_RESULT_DIRECTORY_NOT_EMPTY:
            return ENOTEMPTY;
        case CNH_RESULT_OPERATION_NOT_PERMITTED:
            return EPERM;
        case CNH_RESULT_NO_SUCH_DEVICE:
            return ENODEV;
        case CNH_RESULT_BAD_FILE_NUMBER:
            return EBADF;
        case CNH_RESULT_FILE_DESCRIPTOR_IN_BAD_STATE:
            return EBADFD;
        case CNH_RESULT_VALUE_TOO_LARGE_FOR_DEFINED_DATATYPE:
            return EOVERFLOW;
        case CNH_RESULT_NO_SUCH_DEVICE_OR_ADDRESS:
            return ENXIO;
        case CNH_RESULT_BROKEN_PIPE:
            return EPIPE;
        case CNH_RESULT_TIMER_EXPIRED:
            return ETIME;
        default:
            log_warn("Unhandled other error %d to errno convert might cause bugs", result);
            return EIO;
    }
}

enum cnh_result cnh_errno_to_result(int errn)
{
    switch (errn) {
        case 0:
            return CNH_RESULT_SUCCESS;
        case EINVAL:
            return CNH_RESULT_INVALID_PARAMETER;
        case ENOMEM:
            return CNH_RESULT_OUT_OF_MEMORY;
        case EAGAIN:
            return CNH_RESULT_REPEAT_OPERATION;
        case ENOENT:
            return CNH_RESULT_NO_SUCH_FILE_OR_DIR;
        case ENOTTY:
            return CNH_RESULT_NOT_TTY;
        case EBUSY:
            return CNH_RESULT_BUSY;
        case EISDIR:
            return CNH_RESULT_IS_A_DIRECTORY;
        case EACCES:
            return CNH_RESULT_PERMISSION_DENIED;
        case ENOTEMPTY:
            return CNH_RESULT_DIRECTORY_NOT_EMPTY;
        case EPERM:
            return CNH_RESULT_OPERATION_NOT_PERMITTED;
        case ENODEV:
            return CNH_RESULT_NO_SUCH_DEVICE;
        case EBADF:
            return CNH_RESULT_BAD_FILE_NUMBER;
        case EBADFD:
            return CNH_RESULT_FILE_DESCRIPTOR_IN_BAD_STATE;
        case EOVERFLOW:
            return CNH_RESULT_VALUE_TOO_LARGE_FOR_DEFINED_DATATYPE;
        case ENXIO:
            return CNH_RESULT_NO_SUCH_DEVICE_OR_ADDRESS;
        case EPIPE:
            return CNH_RESULT_BROKEN_PIPE;
        case ETIME:
            return CNH_RESULT_TIMER_EXPIRED;
        default:
            log_warn("Unhandled errno to general error convert might cause bugs, errno: %d", errn);
            return CNH_RESULT_OTHER_ERROR;
    }
}