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
typedef void (*glDrawPixels_t)(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);

static XCreateWindow_t patch_gfx_real_XCreateWindow;
static XOpenDisplay_t patch_gfx_real_XOpenDisplay;
static glXChooseVisual_t patch_gfx_real_GlXChooseVisual;
static glXSwapBuffers_t patch_gfx_real_glXSwapBuffers;
static glDrawPixels_t patch_gfx_real_glDrawPixels;

static bool patch_gfx_initialized;
static uint16_t s_frame_limit = 0;
static uint16_t s_init_display_width = 0;
static uint16_t s_init_display_height = 0;
static uint16_t s_target_display_width = 0;
static uint16_t s_target_display_height = 0;
static uint8_t s_scaling_mode = 0;
static uint8_t s_resizable_window = 0;
static uint8_t s_res_adjust_enabled = 0;
static float s_zoom_factor_x = 0.0f;
static float s_zoom_factor_y = 0.0f;


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

void calculate_zoom_factors(int src_width, int src_height, int dest_width, int dest_height, float *zoom_x, float *zoom_y) {
    *zoom_x = (float) dest_width / (float) src_width;
    *zoom_y = (float) dest_height / (float) src_height;
}

// Check if our window dimensions changed and update our settings.
void GetCurrentWindowDimensions(Display *dpy,GLXDrawable drawable){
    Window root;
    int x, y;
    unsigned int width, height, border_width, depth;
    XGetGeometry(dpy, drawable, &root, &x, &y, &width, &height, &border_width, &depth);
	if(width != s_target_display_width || height != s_target_display_height){		
		s_target_display_width = width;
		s_target_display_height = height;
		// Recalculate Zoom Factor
		calculate_zoom_factors(s_init_display_width,s_init_display_height,s_target_display_width,s_target_display_height,&s_zoom_factor_x,&s_zoom_factor_y);
	}
}

// A modified version of the S3DResize function from the engine to support multimode adjustment.
void S3DResizeEx(void){
  if(s_scaling_mode == PATCH_GFX_SCALE_MODE_STRETCH){
    glViewport(0, 0, s_target_display_width, s_target_display_height);
  }else if(s_scaling_mode == PATCH_GFX_SCALE_MODE_PIXEL_PERFECT){
    float targetAspectRatio = s_init_display_width / (float)s_init_display_height;

    // Figure out the largest area that fits in this resolution at the desired aspect ratio
    int width = s_target_display_width;
    int height = (int)(width / targetAspectRatio + 0.5f);

    if (height > s_target_display_height) {
        // It doesn't fit our height, we must switch to pillarbox then
        height = s_target_display_height;
        width = (int)(height * targetAspectRatio + 0.5f);
    }

    // Calculate pillarbox dimensions
    int pillarbox_x = (s_target_display_width - width) / 2;
    int pillarbox_y = (s_target_display_height - height) / 2;
    if(width != s_target_display_width || height != s_target_display_height){
      s_target_display_width = width;
      s_target_display_height = height;
      calculate_zoom_factors(s_init_display_width,s_init_display_height,s_target_display_width,s_target_display_height,&s_zoom_factor_x,&s_zoom_factor_y);
    }

    
    // Set up the new viewport with pillarboxing
    glViewport(pillarbox_x, pillarbox_y, width, height);

  }
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, s_init_display_width, 0.0f, s_init_display_height, -500.0f, 500.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();  
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
    // Cache startup dimensions.
    s_init_display_width = width;
    s_init_display_height = height;
    // If our target dimensions were zero, we will disable the flag to do any resizing.
    if(s_target_display_height == 0){
      s_target_display_height = height;
    }
    if(s_target_display_width == 0){
      s_target_display_width = width;
    }  
    // We also need zoom factor
    calculate_zoom_factors(s_init_display_width,s_init_display_height,s_target_display_width,s_target_display_height,&s_zoom_factor_x,&s_zoom_factor_y);  

    // If we're not using the resizable window flag and our resolutions match, we won't do any adjustment.
    if(s_resizable_window != 0 || s_target_display_width != width || s_target_display_height != height){
      s_res_adjust_enabled = 1;
    }
    width = s_target_display_width;
    height = s_target_display_height;
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

// Check for fullscreen draw calls and scale them appropriately.
void glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) {
  if (!patch_gfx_real_glDrawPixels) {
    patch_gfx_real_glDrawPixels =
        (glDrawPixels_t) cnh_lib_get_func_addr("glDrawPixels");
  }
    // If our texture is the original screen size, we'll zoom, flip it because reasons, and then draw
    // it to our updated zoom factor.    
    if (width == s_init_display_width && height == s_init_display_height) {
        // Do this - I don't remember why but do it.        
        glRasterPos2i(0,0);       
        glPixelZoom(s_zoom_factor_x, s_zoom_factor_y);
        
        // Create a new buffer to store the flipped image data
        GLubyte *flippedPixels = malloc(width * height * sizeof(GLubyte) * 3);

        // Flip the image data
        for (int i = 0; i < height; i++) {
            GLubyte *srcLine = ((GLubyte *)pixels) + i * width * 3;
            GLubyte *dstLine = flippedPixels + (height - i - 1) * width * 3;
            memcpy(dstLine, srcLine, width * 3);
        }
        // Draw our Updated Image Data
        patch_gfx_real_glDrawPixels(width, height, format, type, flippedPixels);
        // Free the flipped image data buffer
        free(flippedPixels);
        // Reset pixel zoom
	      glPixelZoom(1.0, 1.0);

    }else{
        // Call the original glDrawPixels function
        patch_gfx_real_glDrawPixels(width, height, format, type, pixels);
    }
}

void glXSwapBuffers(Display *dpy, GLXDrawable drawable) {
  if (!patch_gfx_real_glXSwapBuffers) {
    patch_gfx_real_glXSwapBuffers =
        (glXSwapBuffers_t) cnh_lib_get_func_addr("glXSwapBuffers");
  }
  // If we use a resizable window, we must refresh the target dimensions.
  if(s_resizable_window){
    GetCurrentWindowDimensions(dpy,drawable);
  }
  
  if(s_scaling_mode){
    S3DResizeEx();
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

void patch_gfx_display(uint8_t scaling_mode, uint8_t resizable_window, uint16_t screen_width, uint16_t screen_height, uint16_t frame_limit){
  s_scaling_mode = scaling_mode;
  s_resizable_window = resizable_window;
  s_target_display_width = screen_width;
  s_target_display_height = screen_height;
  s_frame_limit = frame_limit;
}
