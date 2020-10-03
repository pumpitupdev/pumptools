/**
 * Results for returning errors on hook modules, converting them to errno and vice versa
 *
 * Based on original capnhook code for windows by decafcode:
 * https://github.com/decafcode/capnhook
 */
#ifndef CAPNHOOK_RESULT_H
#define CAPNHOOK_RESULT_H

/**
 * Result values for hook functions
 */
enum cnh_result {
    CNH_RESULT_SUCCESS = 0,
    CNH_RESULT_ERROR_INSUFFICIENT_BUFFER = 1,
    CNH_RESULT_INVALID_PARAMETER = 2,
    CNH_RESULT_OUT_OF_MEMORY = 3,
    CNH_RESULT_REPEAT_OPERATION = 4,
    CNH_RESULT_NO_SUCH_FILE_OR_DIR = 5,
    CNH_RESULT_OTHER_ERROR = 6,
    CNH_RESULT_NOT_TTY = 7,
    CNH_RESULT_BUSY = 8,
    CNH_RESULT_IS_A_DIRECTORY = 9,
    CNH_RESULT_PERMISSION_DENIED = 10,
    CNH_RESULT_DIRECTORY_NOT_EMPTY = 11,
    CNH_RESULT_OPERATION_NOT_PERMITTED = 12,
    CNH_RESULT_NO_SUCH_DEVICE = 13,
    CNH_RESULT_BAD_FILE_NUMBER = 14,
    CNH_RESULT_FILE_DESCRIPTOR_IN_BAD_STATE = 15,
    CNH_RESULT_VALUE_TOO_LARGE_FOR_DEFINED_DATATYPE = 16,
    CNH_RESULT_NO_SUCH_DEVICE_OR_ADDRESS = 17,
    CNH_RESULT_BROKEN_PIPE = 18,
    CNH_RESULT_TIMER_EXPIRED = 19,
};

/**
 * Convert a hook function result to errno
 *
 * @param result Result to convert
 * @return errno value
 */
int cnh_result_to_errno(enum cnh_result result);

/**
 * Convert an errno value to a hook function result
 *
 * @param errn errno value to convert
 * @return Hook function result
 */
enum cnh_result cnh_errno_to_result(int errn);

#endif
