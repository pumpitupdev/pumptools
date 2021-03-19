#define LOG_MODULE "patch-usbfs"

#include <glob.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "capnhook/hook/lib.h"

#include "util/fs.h"
#include "util/log.h"

typedef int (*vsprintf_t)(char *str, const char *format, va_list ap);
typedef int (*sprintf_t)(char *str, const char *format, ...);

static vsprintf_t patch_usbfs_vsprintf_orig;
static sprintf_t patch_usbfs_sprintf_orig;

static const char *patch_usbfs_usb_str = "/sys/bus/usb/devices/";

int sprintf(char *str, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  int ret = vsprintf(str, format, ap);
  va_end(ap);
  return ret;
}

int __sprintf_chk(
    char *dest,
    int flags,
    size_t dest_len_from_compiler,
    const char *format,
    ...)
{
  int ret;
  va_list ap;
  va_start(ap, format);

  /* fuck...why did you guys have to switch from sprintf to sprintf_chk?
     everything was perfectly fine... and you give a shit about "safe" code,
     which doesn't make this mess better... */
  ret = vsprintf(dest, format, ap);

  va_end(ap);
  return ret;
}

int vsprintf(char *str, const char *format, va_list ap)
{
  if (!patch_usbfs_vsprintf_orig) {
    patch_usbfs_vsprintf_orig = (vsprintf_t) cnh_lib_get_func_addr("vsprintf");
  }

  if (!patch_usbfs_sprintf_orig) {
    patch_usbfs_sprintf_orig = (sprintf_t) cnh_lib_get_func_addr("sprintf");
  }

  /* This path does not work on newer kernels (not sure which version it
     started but everything > 3.18 doesn't work) */

  // nx2/nxa path:
  // /sys/bus/usb/devices/%d-%d/%d-%d:1.0/host*/target*:0:0/*:0:0:0/block:sd*
  // fiesta and newer:
  // /sys/bus/usb/devices/%d-%d:1.0/host*/target*:0:0/*:0:0:0/block/sd*

  if (!strncmp(patch_usbfs_usb_str, format, strlen(patch_usbfs_usb_str))) {
    /* unpack argument list, we know we got 4 arguments here
       the second pair is a dupe of the first one */
    uint8_t bus = (uint8_t) va_arg(ap, int);
    uint8_t port = (uint8_t) va_arg(ap, int);

    // Removed because it spams the log. Put it back when debugging usb profile
    // mounting related issues log_debug("Patching lookup of '%s' device node
    // for bus %d, port %d.", format, bus, port);

    /* note: ports start with 1 (not 0) in sysfs */

    /* because the linux kernel likes to change things here often, we have to
       consider different paths. for each path we encountered so far (add more
       if missing from different kernel versions) just
       check if the path exists and use it */

    int counter = 0;

    while (counter <= 2) {
      switch (counter) {
        case 0:
          patch_usbfs_sprintf_orig(
              str,
              "/sys/bus/usb/devices/%d-1.%d:1.0/host*/"
              "target*:0:0/*:0:0:0/block/sd*",
              bus,
              port);
          break;

        case 1:
          patch_usbfs_sprintf_orig(
              str,
              "/sys/bus/usb/devices/%d-%d:1.0/host*/"
              "target*:0:0/*:0:0:0/block/sd*",
              bus,
              port);
          break;

        case 2:
          // Discovered that the string needs to be changed on 5.5.6. Probably
          // changed slightly before that version
          patch_usbfs_sprintf_orig(
              str,
              "/sys/bus/usb/devices/%d-%d/%d-%d:1.0/host*/"
              "target*:0:0/*:0:0:0/block/sd*",
              bus,
              port,
              bus,
              port);
          break;

        default:
          log_error(
              "Could not find path in sys-fs to lookup usb drive block device. "
              "Likely, you have a "
              "kernel version that uses a (slightly) different path. This "
              "(new) path must be added in "
              "order to get usb drives showing up in the game.");
          break;
      }

      glob_t gl;

      // Pump does the same after returning from this hooked call. Because there
      // are different file system layouts depending on the kernel version, try
      // the above list using the same method with glob
      if (!glob(str, 4, NULL, &gl)) {
        globfree(&gl);
        break;
      }

      globfree(&gl);

      counter++;
    }

    /* original argument count */
    return 4;
  }

  return patch_usbfs_vsprintf_orig(str, format, ap);
}

void patch_usbfs_init()
{
  log_info("Initialized");
}