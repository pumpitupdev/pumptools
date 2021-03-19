#define LOG_MODULE "patch-gfx"

#include <X11/Xutil.h>
#include <stdbool.h>
#include <GL/gl.h>

// OpenGL 3.x+
#include <GL/glx.h>

#include "capnhook/hook/lib.h"

#include "util/log.h"
#include "util/str.h"

#include "gfx.h"

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
typedef XVisualInfo *(*glXChooseVisual_t)(
    Display *dpy, int screen, int *attribList);
typedef void (*glViewport_t)(GLint x, GLint y, GLsizei width, GLsizei height);

static bool patch_gfx_initialized;
static XCreateWindow_t patch_gfx_real_XCreateWindow;
static XOpenDisplay_t patch_gfx_real_XOpenDisplay;
static glXChooseVisual_t patch_gfx_real_GlXChooseVisual;
static glViewport_t patch_gfx_real_glViewport;

static bool _patch_gfx_scaling_enabled;
static uint16_t _patch_gfx_screen_width;
static uint16_t _patch_gfx_screen_height;
static uint16_t _patch_gfx_viewport_width;
static uint16_t _patch_gfx_viewport_height;
static uint16_t _patch_gfx_viewport_pos_x;
static uint16_t _patch_gfx_viewport_pos_y;

static char *patch_gfx_attrib_list_to_str(int *attrib_list)
{
  int *ptr = attrib_list;
  char *str = util_str_dup("");
  char buf[32];

  while (*ptr != None) {
    sprintf(buf, "%d, ", *ptr);

    char *tmp = util_str_merge(str, buf);

    free(str);
    str = tmp;

    ptr++;
  }

  return str;
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

  if (!patch_gfx_real_XCreateWindow) {
    patch_gfx_real_XCreateWindow =
        (XCreateWindow_t) cnh_lib_get_func_addr("XCreateWindow");
  }

  if (patch_gfx_initialized) {

    if (valuemask == 0x80A) {
      log_info("Patching to enable usage of non nvidia cards.");
      /* enables usage of non nvidia cards and newer nvidia models */
      valuemask = 0x280A;
    }
  }

  if (_patch_gfx_scaling_enabled) {
    width = _patch_gfx_screen_width;
    height = _patch_gfx_screen_height;
  }

  return patch_gfx_real_XCreateWindow(
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
}

Display *XOpenDisplay(const char *display_name)
{
  if (!patch_gfx_real_XOpenDisplay) {
    patch_gfx_real_XOpenDisplay =
        (XOpenDisplay_t) cnh_lib_get_func_addr("XOpenDisplay");
  }

  log_info("XOpenDisplay: %s", display_name);

  Display *res = patch_gfx_real_XOpenDisplay(display_name);

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

XVisualInfo *glXChooseVisual(Display *dpy, int screen, int *attribList)
{
  if (!patch_gfx_real_GlXChooseVisual) {
    patch_gfx_real_GlXChooseVisual =
        (glXChooseVisual_t) cnh_lib_get_func_addr("glXChooseVisual");
  }

  char *attrib_list_str = patch_gfx_attrib_list_to_str(attribList);

  log_info(
      "glXChooseVisual: dpy %p, screen %d, attribList %s",
      dpy,
      screen,
      attrib_list_str);

  free(attrib_list_str);

  XVisualInfo *res = patch_gfx_real_GlXChooseVisual(dpy, screen, attribList);

  if (!res) {
    log_error(
        "glXChooseVisual returned NULL. It is likely that your current "
        "environment does support hardware"
        "acceleration. Either you do not have any GPU drivers installed or the "
        "drivers are not configured "
        "properly. Fix that and use the command line tool \"glxinfo\" for "
        "debugging this.");
  }

  return res;
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  if (!patch_gfx_real_glViewport) {
    void* handle = cnh_lib_load("libGL.so");

    patch_gfx_real_glViewport =
        (glViewport_t) cnh_lib_get_func_addr_handle(handle, "glViewport");
  }

  // 0/0 at the bottom left
  if (_patch_gfx_scaling_enabled) {
    x = _patch_gfx_viewport_pos_x;
    y = _patch_gfx_viewport_pos_y;
    width = _patch_gfx_viewport_width;
    height = _patch_gfx_viewport_height;
  }

  patch_gfx_real_glViewport(x, y, width, height);
}

void patch_gfx_init()
{
  _patch_gfx_scaling_enabled = false;
  patch_gfx_initialized = true;
  log_info("Initialized");
}

// Note, consider improving scaling, see http://www.david-amador.com/2013/04/opengl-2d-independent-resolution-rendering/
void patch_gfx_scale(enum patch_gfx_scale_mode scale_mode)
{
  switch (scale_mode) {
    case PATCH_GFX_SCALE_MODE_SD_480_TO_PILLARBOX_HD_720:
      _patch_gfx_screen_width = 1280;
      _patch_gfx_screen_height = 720;
      _patch_gfx_viewport_width = 960;
      _patch_gfx_viewport_height = 720;
      _patch_gfx_viewport_pos_x = 160;
      _patch_gfx_viewport_pos_y = 0;
      break;

    case PATCH_GFX_SCALE_MODE_SD_480_TO_PILLARBOX_HD_1080:
      _patch_gfx_screen_width = 1920;
      _patch_gfx_screen_height = 1080;
      _patch_gfx_viewport_width = 1440;
      _patch_gfx_viewport_height = 1080;
      _patch_gfx_viewport_pos_x = 240;
      _patch_gfx_viewport_pos_y = 0;
      break;

    case PATCH_GFX_SCALE_MODE_SD_480_TO_SD_960:
      _patch_gfx_screen_width = 1280;
      _patch_gfx_screen_height = 960;
      _patch_gfx_viewport_width = 1280;
      _patch_gfx_viewport_height = 960;
      _patch_gfx_viewport_pos_x = 0;
      _patch_gfx_viewport_pos_y = 0;
      break;

    case PATCH_GFX_SCALE_MODE_HD_720_TO_HD_1080:
      _patch_gfx_screen_width = 1920;
      _patch_gfx_screen_height = 1080;
      _patch_gfx_viewport_width = 1920;
      _patch_gfx_viewport_height = 1080;
      _patch_gfx_viewport_pos_x = 0;
      _patch_gfx_viewport_pos_y = 0;
      break;

    case PATCH_GFX_SCALE_MODE_INVALID:
    default:
      log_error("Invalid scale mode: %d. Scaling disabled", scale_mode);
      return;
  }

  log_info("Scaling enabled, mode: %d", scale_mode);

  _patch_gfx_scaling_enabled = true;
}
