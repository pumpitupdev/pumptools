/**
 * Hook and intercept signal handlers
 */
#ifndef CAPNHOOK_SIG_H
#define CAPNHOOK_SIG_H

#include <signal.h>

/**
 * Special callback for signal allowing access to a signal handler installed
 * by the application
 *
 * @param signal Signal to handle
 * @param orig_handler Original handler of application or NULL if non exists
 */
typedef void (*cnh_sig_handler_t)(int signal, __sighandler_t orig_handler);

/**
 * Install our own signal handler (and possibly override existing handlers)
 *
 * @param sig Signal to handle
 * @param handler Handler to install for specified signal
 */
void cnh_sig_install(int sig, cnh_sig_handler_t handler);

#endif