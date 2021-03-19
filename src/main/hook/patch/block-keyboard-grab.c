#define LOG_MODULE "block-keyboard-grab"

#include <X11/Xutil.h>
#include <stdbool.h>

#include "capnhook/hook/lib.h"

#include "util/log.h"

typedef int (*XGrabKeyboard_t)(
    Display *display,
    Window window,
    Bool owner_events,
    int pointer_mode,
    int keyboard_mode,
    Time time);

static XGrabKeyboard_t _patch_main_loop_real_XGrabKeyboard;

static bool _patch_block_keyboard_grab_enable;

int XGrabKeyboard(
    Display *display,
    Window window,
    Bool owner_events,
    int pointer_mode,
    int keyboard_mode,
    Time time)
{
  if (!_patch_main_loop_real_XGrabKeyboard) {
    _patch_main_loop_real_XGrabKeyboard =
        (XGrabKeyboard_t) cnh_lib_get_func_addr("XGrabKeyboard");
  }

  if (_patch_block_keyboard_grab_enable) {
    log_debug("Block keyboard grabbing");
    return 0;
  }

  return _patch_main_loop_real_XGrabKeyboard(
      display, window, owner_events, pointer_mode, keyboard_mode, time);
}

void patch_block_keyboard_grab_init()
{
  _patch_block_keyboard_grab_enable = true;

  log_info("Initialized");
}