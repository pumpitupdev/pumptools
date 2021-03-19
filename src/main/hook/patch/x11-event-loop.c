#define LOG_MODULE "patch-x11-event-loop"

#include <X11/Xutil.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>

#include "capnhook/hook/lib.h"

#include "ptapi/io/x11-input-hook.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/time.h"

struct patch_x11_event_loop_ctx {
  Display *display;
  Window window;
  pthread_t thread;
  atomic_int run;
};

typedef Window (*XCreateWindow_t)(
    Display *display,
    Window parent,
    int x,
    int y,
    unsigned int width,
    unsigned int height,
    unsigned int border_width,
    int depth,
    unsigned int _class,
    Visual *visual,
    unsigned long valuemask,
    XSetWindowAttributes *attributes);
typedef Display *(*XOpenDisplay_t)(const char *display_name);
typedef int (*XDestroyWindow_t)(Display *display, Window w);

static XCreateWindow_t patch_x11_event_loop_real_XCreateWindow;
static XOpenDisplay_t patch_x11_event_loop_real_XOpenDisplay;
static XDestroyWindow_t patch_x11_event_loop_real_XDestroyWindow;

static bool patch_x11_event_loop_enabled;
static struct patch_x11_event_loop_ctx *patch_x11_event_loop_ctx;

static pthread_mutex_t patch_x11_event_loop_input_handlers_lock;
static const struct ptapi_io_x11_input_hook_handler *
    *patch_x11_event_loop_input_handlers;
static size_t patch_x11_event_loop_num_input_handlers;

static bool
patch_x11_event_loop_is_key_repeated(Display *display, XEvent *event)
{
  /* When a key is repeated, there will be two events: released, followed by
     another immediate pressed. So check to see if another pressed is present */
  if (!XPending(display)) {
    return false;
  }

  XEvent e;
  XPeekEvent(display, &e);

  if (e.type == KeyPress && e.xkey.keycode == event->xkey.keycode &&
      (e.xkey.time - event->xkey.time) < 2) {
    XNextEvent(display, &e);
    return true;
  }

  return false;
}

static void *patch_x11_event_loop_run(void *args)
{
  log_info("Running X11 event loop thread");

  struct patch_x11_event_loop_ctx *ctx =
      (struct patch_x11_event_loop_ctx *) args;

  if (XSelectInput(ctx->display, ctx->window, KeyPressMask | KeyReleaseMask) ==
      BadWindow) {
    log_error("XSelectInput failed");
    return NULL;
  }

  XGrabKeyboard(
      ctx->display, ctx->window, 1, GrabModeAsync, GrabModeAsync, CurrentTime);

  while (atomic_load(&ctx->run) > 0) {
    while (atomic_load(&ctx->run) > 0 && XPending(ctx->display) > 0) {
      XEvent event;
      KeySym key;

      if (XNextEvent(ctx->display, &event) != Success) {
        log_error("XNextEvent");
        continue;
      }

      switch (event.type) {
        case KeyPress: {
          /* Mask out the modifier states X11 sets and read again */
          event.xkey.state &= ~ShiftMask;
          event.xkey.state &= ~LockMask;

          XLookupString(&event.xkey, 0, 0, &key, 0);

          /* Dispatch to external input handlers */
          if (patch_x11_event_loop_num_input_handlers > 0) {
            pthread_mutex_lock(&patch_x11_event_loop_input_handlers_lock);

            for (size_t i = 0; i < patch_x11_event_loop_num_input_handlers;
                 i++) {
              if (patch_x11_event_loop_input_handlers[i]->dispatch_key_press) {
                patch_x11_event_loop_input_handlers[i]->dispatch_key_press(key);
              }
            }

            pthread_mutex_unlock(&patch_x11_event_loop_input_handlers_lock);
          }

          break;
        }

        case KeyRelease: {
          /* Mask out the modifier states X11 sets and read again */
          event.xkey.state &= ~ShiftMask;
          event.xkey.state &= ~LockMask;

          XLookupString(&event.xkey, 0, 0, &key, 0);

          /* Dispatch to external input handlers */
          if (patch_x11_event_loop_num_input_handlers > 0) {
            /* Dispatch release if key is not repeated */
            if (!patch_x11_event_loop_is_key_repeated(ctx->display, &event)) {
              pthread_mutex_lock(&patch_x11_event_loop_input_handlers_lock);

              for (size_t i = 0; i < patch_x11_event_loop_num_input_handlers;
                   i++) {
                if (patch_x11_event_loop_input_handlers[i]
                        ->dispatch_key_release) {
                  patch_x11_event_loop_input_handlers[i]->dispatch_key_release(
                      key);
                }
              }

              pthread_mutex_unlock(&patch_x11_event_loop_input_handlers_lock);
            }
          }

          break;
        }

        default:
          break;
      }
    }

    // Avoid CPU banging
    util_time_sleep_ms(1);
  }

  log_info("Finished X11 event loop thread");

  return NULL;
}

Display *XOpenDisplay(const char *display_name)
{
  if (!patch_x11_event_loop_real_XOpenDisplay) {
    patch_x11_event_loop_real_XOpenDisplay =
        (XOpenDisplay_t) cnh_lib_get_func_addr("XOpenDisplay");
  }

  log_debug("XOpenDisplay: %s", display_name);

  // This function needs to be called before any other X11 function is called.
  // Required to pump events in separate thread
  if (patch_x11_event_loop_enabled && XInitThreads() != Success) {
    log_error("XInitThreads failed");
  }

  Display *res = patch_x11_event_loop_real_XOpenDisplay(display_name);

  if (!res) {
    log_error(
        "XOpenDisplay returned NULL. This might indicate that your environment "
        "is not properly setup. Check "
        " that you have GPU drivers installed and configured properly and that "
        "OpenGL hardware acceleration is "
        " working (use the command line tool \"glxinfo\").");
  }

  return res;
}

Window XCreateWindow(
    Display *display,
    Window parent,
    int x,
    int y,
    unsigned int width,
    unsigned int height,
    unsigned int border_width,
    int depth,
    unsigned int _class,
    Visual *visual,
    unsigned long valuemask,
    XSetWindowAttributes *attributes)
{
  log_debug("XCreateWindow");

  if (!patch_x11_event_loop_real_XCreateWindow) {
    patch_x11_event_loop_real_XCreateWindow =
        (XCreateWindow_t) cnh_lib_get_func_addr("XCreateWindow");
  }

  Window w = patch_x11_event_loop_real_XCreateWindow(
      display,
      parent,
      x,
      y,
      width,
      height,
      border_width,
      depth,
      _class,
      visual,
      valuemask,
      attributes);

  if (patch_x11_event_loop_enabled && !patch_x11_event_loop_ctx) {
    patch_x11_event_loop_ctx =
        util_xmalloc(sizeof(struct patch_x11_event_loop_ctx));

    patch_x11_event_loop_ctx->display = display;
    patch_x11_event_loop_ctx->window = w;
    patch_x11_event_loop_ctx->run = ATOMIC_VAR_INIT(1);

    pthread_create(
        &patch_x11_event_loop_ctx->thread,
        NULL,
        patch_x11_event_loop_run,
        patch_x11_event_loop_ctx);
  }

  return w;
}

int XDestroyWindow(Display *display, Window w)
{
  log_debug("XDestroyWindow");

  if (!patch_x11_event_loop_real_XDestroyWindow) {
    patch_x11_event_loop_real_XDestroyWindow =
        (XDestroyWindow_t) cnh_lib_get_func_addr("XDestroyWindow");
  }

  if (patch_x11_event_loop_enabled && patch_x11_event_loop_ctx) {
    log_debug("Stopping X11 event loop thread...");

    atomic_store(&patch_x11_event_loop_ctx->run, 0);
    pthread_join(patch_x11_event_loop_ctx->thread, NULL);
    free(patch_x11_event_loop_ctx);
    patch_x11_event_loop_ctx = NULL;

    log_debug("Stopped X11 event loop thread");
  }

  return patch_x11_event_loop_real_XDestroyWindow(display, w);
}

void patch_x11_event_loop_init()
{
  patch_x11_event_loop_enabled = true;
  log_info("Initialized");
}

void patch_x11_event_loop_add_input_handler(const char *lib_with_handler_impl)
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

  pthread_mutex_lock(&patch_x11_event_loop_input_handlers_lock);

  new_size = patch_x11_event_loop_num_input_handlers + 1;
  new_array = realloc(
      patch_x11_event_loop_input_handlers,
      new_size * sizeof(struct ptapi_io_input_hook_handler *));

  if (new_array != NULL) {
    patch_x11_event_loop_input_handlers = new_array;

    patch_x11_event_loop_input_handlers
        [patch_x11_event_loop_num_input_handlers++] = input_handler_hook;
  } else {
    log_die("Out of memory");
  }

  pthread_mutex_unlock(&patch_x11_event_loop_input_handlers_lock);

  log_debug(
      "Added input handler %p of lib %s",
      input_handler_hook,
      lib_with_handler_impl);
}