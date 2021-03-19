#define LOG_MODULE "patch-main-loop"

#include <X11/Xutil.h>
#include <pthread.h>
#include <stdbool.h>

#include "ptapi/io/x11-input-hook.h"

#include "capnhook/hook/lib.h"
#include "capnhook/hook/sig.h"

#include "hook/patch/main-loop.h"

#include "util/log.h"

typedef int (*XPending_t)(Display *display);
typedef int (*XNextEvent_t)(Display *display, XEvent *event_return);

typedef const struct ptapi_io_x11_input_hook_handler *(
    *ptapi_io_x11_input_handler_hook_t)(void);

static XPending_t _patch_main_loop_real_XPending;
static XNextEvent_t _patch_main_loop_real_XNextEvent;

static void
_patch_main_loop_sigalarm_handler(int signal, __sighandler_t orig_handler);
static bool _patch_main_loop_is_key_repeated(Display *display, XEvent *event);

static __sighandler_t _patch_main_loop_sigalarm_orig_handler;
static bool _patch_main_loop_disable_built_in_inputs;
static bool _patch_main_loop_disable_mk3_linux_ports_built_in_inputs;
static pthread_mutex_t _patch_main_loop_input_handlers_lock;
static const struct ptapi_io_x11_input_hook_handler *
    *_patch_main_loop_input_handlers;
static size_t _patch_main_loop_num_input_handlers;

void patch_main_loop_init(
    bool fix_sigalarm_main_loop,
    bool disable_built_in_inputs,
    bool disable_mk3_linux_port_inputs)
{
  _patch_main_loop_disable_built_in_inputs = disable_built_in_inputs;
  _patch_main_loop_disable_mk3_linux_ports_built_in_inputs =
      disable_mk3_linux_port_inputs;

  _patch_main_loop_sigalarm_orig_handler = NULL;

  if (fix_sigalarm_main_loop) {
    log_info("Applying sigalarm handler fix");
    cnh_sig_install(SIGALRM, _patch_main_loop_sigalarm_handler);
  }

  pthread_mutex_init(&_patch_main_loop_input_handlers_lock, NULL);

  log_info("Initialized, disable built-in inputs %d", disable_built_in_inputs);
}

void patch_main_loop_add_x11_input_handler(const char *lib_with_handler_impl)
{
  const struct ptapi_io_x11_input_hook_handler **new_array;
  size_t new_size;
  void *lib_handle;
  ptapi_io_x11_input_handler_hook_t input_handler_hook_func;
  const struct ptapi_io_x11_input_hook_handler *input_handler_hook;

  if (!lib_with_handler_impl) {
    log_die("No library path with input handler implementation specified");
  }

  log_info(
      "Loading library with x11-input-handler api implementation %s",
      lib_with_handler_impl);

  lib_handle = cnh_lib_load(lib_with_handler_impl);

  input_handler_hook_func =
      (ptapi_io_x11_input_handler_hook_t) cnh_lib_get_func_addr_handle(
          lib_handle, "ptapi_io_x11_input_handler_hook");

  input_handler_hook = input_handler_hook_func();

  if (!input_handler_hook) {
    log_warn(
        "Library impl %s returned NULL hook handler, skipping",
        lib_with_handler_impl);
    return;
  }

  pthread_mutex_lock(&_patch_main_loop_input_handlers_lock);

  new_size = _patch_main_loop_num_input_handlers + 1;
  new_array = realloc(
      _patch_main_loop_input_handlers,
      new_size * sizeof(struct ptapi_io_input_hook_handler *));

  if (new_array != NULL) {
    _patch_main_loop_input_handlers = new_array;

    _patch_main_loop_input_handlers[_patch_main_loop_num_input_handlers++] =
        input_handler_hook;
  } else {
    log_die("Out of memory");
  }

  pthread_mutex_unlock(&_patch_main_loop_input_handlers_lock);

  log_debug(
      "Added input handler %p of lib %s",
      input_handler_hook,
      lib_with_handler_impl);
}

int XPending(Display *display)
{
  if (!_patch_main_loop_real_XPending) {
    _patch_main_loop_real_XPending =
        (XPending_t) cnh_lib_get_func_addr("XPending");
  }

  /* poll the original SIGALRM handler to drive the pump engine */
  if (_patch_main_loop_sigalarm_orig_handler) {
    _patch_main_loop_sigalarm_orig_handler(SIGALRM);
  }

  return _patch_main_loop_real_XPending(display);
}

int XNextEvent(Display *display, XEvent *event_return)
{
  int res;
  KeySym key;

  if (!_patch_main_loop_real_XNextEvent) {
    _patch_main_loop_real_XNextEvent =
        (XNextEvent_t) cnh_lib_get_func_addr("XNextEvent");
  }

  /* Trap KeyPress and KeyRelease events */
  res = _patch_main_loop_real_XNextEvent(display, event_return);

  switch (event_return->type) {
    case KeyPress: {
      /* Mask out the modifier states X11 sets and read again */
      event_return->xkey.state &= ~ShiftMask;
      event_return->xkey.state &= ~LockMask;

      XLookupString(&event_return->xkey, 0, 0, &key, 0);

      /* Swallow some keys to disable them */
      if (_patch_main_loop_disable_built_in_inputs) {
        if (key == XK_F1 || key == XK_F2 || key == XK_F3) {
          event_return->type = GenericEvent;
        }
      }

      /* Swallow all built-in inputs of the MK3 ports */
      if (_patch_main_loop_disable_mk3_linux_ports_built_in_inputs) {
        if (key == XK_g || key == XK_h || key == XK_j || key == XK_k ||
            key == XK_l || key == XK_q || key == XK_e || key == XK_s ||
            key == XK_z || key == XK_c || key == XK_KP_7 || key == XK_KP_9 ||
            key == XK_KP_5 || key == XK_KP_1 || key == XK_KP_3) {
          event_return->type = GenericEvent;
        }
      }

      /* Dispatch to external input handlers */
      if (_patch_main_loop_num_input_handlers > 0) {
        pthread_mutex_lock(&_patch_main_loop_input_handlers_lock);

        for (size_t i = 0; i < _patch_main_loop_num_input_handlers; i++) {
          if (_patch_main_loop_input_handlers[i]->dispatch_key_press) {
            _patch_main_loop_input_handlers[i]->dispatch_key_press(key);
          }
        }

        pthread_mutex_unlock(&_patch_main_loop_input_handlers_lock);
      }

      break;
    }

    case KeyRelease: {
      /* Mask out the modifier states X11 sets and read again */
      event_return->xkey.state &= ~ShiftMask;
      event_return->xkey.state &= ~LockMask;

      XLookupString(&event_return->xkey, 0, 0, &key, 0);

      /* Swallow some keys to disable them */
      if (_patch_main_loop_disable_built_in_inputs) {
        if (key == XK_F1 || key == XK_F2 || key == XK_F3) {
          event_return->type = GenericEvent;
        }
      }

      /* Swallow all built-in inputs of the MK3 ports */
      if (_patch_main_loop_disable_mk3_linux_ports_built_in_inputs) {
        if (key == XK_g || key == XK_h || key == XK_j || key == XK_k ||
            key == XK_l || key == XK_q || key == XK_e || key == XK_s ||
            key == XK_z || key == XK_c || key == XK_KP_7 || key == XK_KP_9 ||
            key == XK_KP_5 || key == XK_KP_1 || key == XK_KP_3) {
          event_return->type = GenericEvent;
        }
      }

      /* Dispatch to external input handlers */
      if (_patch_main_loop_num_input_handlers > 0) {
        /* Dispatch release if key is not repeated */
        if (!_patch_main_loop_is_key_repeated(display, event_return)) {
          pthread_mutex_lock(&_patch_main_loop_input_handlers_lock);

          for (size_t i = 0; i < _patch_main_loop_num_input_handlers; i++) {
            if (_patch_main_loop_input_handlers[i]->dispatch_key_release) {
              _patch_main_loop_input_handlers[i]->dispatch_key_release(key);
            }
          }

          pthread_mutex_unlock(&_patch_main_loop_input_handlers_lock);
        }
      }

      break;
    }

    default:
      break;
  }

  return res;
}

static void
_patch_main_loop_sigalarm_handler(int signal, __sighandler_t orig_handler)
{
  /* do nothing on sigalrm */
  _patch_main_loop_sigalarm_orig_handler = orig_handler;
}

static bool _patch_main_loop_is_key_repeated(Display *display, XEvent *event)
{
  /* When a key is repeated, there will be two events: released, followed by
     another immediate pressed. So check to see if another pressed is present */
  if (!_patch_main_loop_real_XPending(display)) {
    return false;
  }

  XEvent e;
  XPeekEvent(display, &e);

  if (e.type == KeyPress && e.xkey.keycode == event->xkey.keycode &&
      (e.xkey.time - event->xkey.time) < 2) {
    _patch_main_loop_real_XNextEvent(display, &e);
    return true;
  }

  return false;
}