#define LOG_MODULE "patch-redir"

#include "capnhook/hooklib/redir.h"

#include "util/log.h"

void patch_redir_init()
{
    cnh_fshook_push_handler(cnh_redir_fshook);
    cnh_filehook_push_handler(cnh_redir_filehook);
    cnh_iohook_push_handler(cnh_redir_iohook);

    log_info("Initialized");
}