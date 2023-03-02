#define LOG_MODULE "patch-gfx"

#include <GL/gl.h>
#include <X11/Xutil.h>
#include <stdbool.h>

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

static bool patch_gfx_initialized;
static XCreateWindow_t patch_gfx_real_XCreateWindow;
static XOpenDisplay_t patch_gfx_real_XOpenDisplay;
static glXChooseVisual_t patch_gfx_real_GlXChooseVisual;

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
    // During early Exceed-Era code, CWColorMap was not included as a valuemask for creation of a window.
    // Eventually, this was brought back, but it's suspected that NVIDIA cards, drivers at the time, or 
    // perhaps even a combination of that and X11 supported querying the gpu itself for a colormap. 
    // Either way, this no longer works consistently and we need to account for that.
    if ((valuemask & CWColormap) == 0) {
      log_info("Adding CWColorMap to ValueMask for Non-NVIDIA Cards.");
      /* enables usage of nvidia cards where hardware colormaps are not available or supported */
      valuemask |= CWColormap;
    }
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

void patch_gfx_init()
{
  patch_gfx_initialized = true;
  log_info("Initialized");
}

// Note, consider improving scaling, see
// http://www.david-amador.com/2013/04/opengl-2d-independent-resolution-rendering/
void patch_gfx_scale(enum patch_gfx_scale_mode scale_mode)
{
  log_error(
      "GFX scaling enabled but will not work. Temporarily removed due to "
      "issues with loading libGL.so");
}
