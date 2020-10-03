#define LOG_MODULE "cnh-sig"

#include "capnhook/hook/lib.h"
#include "capnhook/hook/sig.h"

#include "util/log.h"

typedef int (*cnh_sig_handler_sigaction_t) (int sig, const struct sigaction* act, struct sigaction* oact);

static void _cnh_signal_handler(int sig);

static cnh_sig_handler_sigaction_t _cnh_sig_handler_real_sigaction;

static cnh_sig_handler_t _cnh_signal_handlers[32];
static __sighandler_t _cnh_signal_orig_handlers[32];

void cnh_sig_install(int sig, cnh_sig_handler_t handler)
{
    if (sig < 0 || sig >= 32) {
        log_die("Can't register handler for invalid signal number %d", sig);
    }

    /* Ensure that the application does not install it's own handler */
    _cnh_signal_handlers[sig] = handler;

    /* Install a little detour as our signal handler */
    signal(sig, _cnh_signal_handler);

    log_info("Installed signal handler %p for %d", handler, sig);
}

int sigaction(int sig, const struct sigaction* act, struct sigaction* oact)
{
    if (_cnh_signal_handlers[sig]) {
        log_info("Enforce own signal handler on signal: %d", sig);

        /* save the original handler, we might need it */
        _cnh_signal_orig_handlers[sig] = ((struct sigaction*) act)->sa_handler;

        /* swap */
        ((struct sigaction*) act)->sa_handler = (__sighandler_t) _cnh_signal_handler;

        log_debug("Original handler %p for signal %d", _cnh_signal_orig_handlers[sig], sig);
    }

	if (!_cnh_sig_handler_real_sigaction) {
        _cnh_sig_handler_real_sigaction = (cnh_sig_handler_sigaction_t) cnh_lib_get_func_addr("sigaction");
    }

    return _cnh_sig_handler_real_sigaction(sig, act, oact);
}

static void _cnh_signal_handler(int sig)
{
    if (_cnh_signal_handlers[sig]) {
        _cnh_signal_handlers[sig](sig, _cnh_signal_orig_handlers[sig]);
    }
}