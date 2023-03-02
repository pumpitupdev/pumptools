#define LOG_MODULE "patch-gfx"

#include <X11/Xutil.h>
#include <stdbool.h>
#include <GL/gl.h>

// OpenGL 3.x+
#include <GL/glx.h>

// For Frame Limiter
#include <unistd.h> 
#include <sys/time.h>

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
typedef void (*glXSwapBuffers_t)(Display *dpy, GLXDrawable drawable);

static bool patch_gfx_initialized;
static XCreateWindow_t patch_gfx_real_XCreateWindow;
static XOpenDisplay_t patch_gfx_real_XOpenDisplay;
static glXChooseVisual_t patch_gfx_real_GlXChooseVisual;
static glXSwapBuffers_t patch_gfx_real_glXSwapBuffers;
static uint16_t s_frame_limit = 0;

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

// Typical sleep until elapsed exit time
void wait_frame_limit(unsigned int fps){
    static unsigned int last_swap_time = 0;
    unsigned int current_time = 0;
    unsigned int swap_interval_usec = 1000000 / fps; // swap every 1/60th of a second
    // Wait until it's time to swap again
    do {
        // Get the current time
        struct timeval tv;
        gettimeofday(&tv, NULL);
        current_time = tv.tv_sec * 1000000 + tv.tv_usec;

        // Calculate the time elapsed since the last swap
        unsigned int elapsed_usec = current_time - last_swap_time;

        // If less than 1/60th of a second has elapsed, wait
        if (elapsed_usec < swap_interval_usec) {
            usleep(swap_interval_usec - elapsed_usec);
        }
    } while (current_time - last_swap_time < swap_interval_usec);

    // Record the time of this swap
    last_swap_time = current_time;
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

void glXSwapBuffers(Display *dpy, GLXDrawable drawable) {
  if (!patch_gfx_real_glXSwapBuffers) {
    patch_gfx_real_glXSwapBuffers =
        (glXSwapBuffers_t) cnh_lib_get_func_addr("glXSwapBuffers");
  }

    patch_gfx_real_glXSwapBuffers(dpy, drawable);
    // We'll wait until the next frame if we need to slow things down.
    if(s_frame_limit){
        wait_frame_limit(s_frame_limit);
    }
}

void patch_gfx_init()
{
  patch_gfx_initialized = true;
  log_info("Initialized");
}

// Note, consider improving scaling, see http://www.david-amador.com/2013/04/opengl-2d-independent-resolution-rendering/
void patch_gfx_scale(enum patch_gfx_scale_mode scale_mode)
{
  log_error("GFX scaling enabled but will not work. Temporarily removed due to issues with loading libGL.so");
}

void patch_gfx_frame_limit(uint16_t frame_limit){
  s_frame_limit = frame_limit;
}