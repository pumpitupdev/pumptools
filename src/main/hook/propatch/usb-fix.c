#define LOG_MODULE "usb-fix"

#include <unistd.h>

#include "capnhook/hook/lib.h"

#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

typedef ssize_t (*readlink_t)(const char *path, char *buf, size_t len);
static readlink_t pro_usb_fix_real_readlink;

static const int16_t propatch_usb_fix_usb_static_ini_usb_bus[2] = {1, 1};
static const int16_t propatch_usb_fix_usb_static_ini_usb_port[2] = {2, 1};

static char propatch_usb_fix_device_thumb_drive_p1[128];
static char propatch_usb_fix_device_thumb_drive_p2[128];

static int propatch_usb_fix_usb_bus[2];
static int propatch_usb_fix_usb_port[2];

static bool propatch_usb_fix_enabled;

// Non static for testing
ssize_t propatch_usb_fix_readlink(const char *path, char *buf, size_t len)
{
  if (!pro_usb_fix_real_readlink) {
    pro_usb_fix_real_readlink = (readlink_t) cnh_lib_get_func_addr("readlink");
  }

  // pro is scanning the /sys/block directory where all usb thumb drives show up
  // once connected The game iterates the directory and resolves the
  // /sys/block/sdX/device symlink On current kernels, e.g. 5.6.14, it might
  // look like this
  // ../devices/pci0000:00/0000:00:14.0/usb2/2-2/2-2:1.0/host7/target7:0:0/7:0:0:0/block/sdd
  // However, when pro was developed, the symlink looked entirely different.
  // In order to allow the game to detect and mount usb thumb drives on newer
  // kernels, the path needs to be altered to match what the game is expecting
  // which might be similar to this:
  // ../devices/pci0000:00/0000:00:14.0/usb2/2-2/2-2:1.0
  // The game tokenizes this path by / and picks the second to last item, which
  // is '2-2' here. This item gets further split into bus (2) and port (2). If
  // that is successful, the game treats the detected device as valid allowing
  // it to mount if the bus and port match what has been configured in the
  // Static.ini file
  if (propatch_usb_fix_enabled && util_str_starts_with(path, "/sys/block/") &&
      util_str_ends_with(path, "/device")) {
    log_debug("Trap %s", path);

    // Remove the trailing "/device" part which is no longer valid on current
    // kernels /sys/block/sdc/device -> /sys/block/sdc
    char *tmp = util_str_dup(path);
    size_t len_tmp = strlen(tmp);
    tmp[len_tmp - 7] = '\0';

    memset(buf, 0, len);

    ssize_t res = pro_usb_fix_real_readlink(tmp, buf, len);

    util_xfree((void **) &tmp);

    if (strstr(buf, propatch_usb_fix_device_thumb_drive_p1) ||
        strstr(buf, propatch_usb_fix_device_thumb_drive_p2)) {
      log_debug("Detected assigned flash drive plugged in");

      size_t count;
      char **toks;

      toks = util_str_split(buf, "/", &count);

      // some sanity checks to ensure the format isn't different
      if (count != 12 || strcmp(toks[0], "..") != 0 ||
          strcmp(toks[1], "devices") != 0 || strcmp(toks[10], "block") != 0) {
        log_error(
            "/sys/block device readlink format changed, usb thumb drive "
            "profiles will likely not work!: %s",
            buf);
        util_str_free_split(toks, count);
        return 0;
      }

      int bus;
      int port;
      const char *device = toks[11];

      if (sscanf(toks[5], "%d-%d", &bus, &port) != 2) {
        log_error("Parsing bus-port of %s failed", buf);
        util_str_free_split(toks, count);
        return 0;
      }

      log_debug(
          "Detected flash drive on bus %d, port %d, device %s",
          bus,
          port,
          device);

      for (int i = 0; i < 2; i++) {
        if (propatch_usb_fix_usb_bus[i] == bus &&
            propatch_usb_fix_usb_port[i] == port) {
          // The usb manager only cares about reading the second to last token
          // in the correct format Copy-paste everything else
          sprintf(
              buf,
              "%s/%s/%s/%s/%s/%d-%d/%d-%d:1.0",
              toks[0],
              toks[1],
              toks[2],
              toks[3],
              toks[4],
              propatch_usb_fix_usb_static_ini_usb_bus[i],
              propatch_usb_fix_usb_static_ini_usb_port[i],
              propatch_usb_fix_usb_static_ini_usb_bus[i],
              propatch_usb_fix_usb_static_ini_usb_port[i]);

          log_debug("Player %d usb thumb drive at path %s", i, buf);

          util_str_free_split(toks, count);

          return strlen(buf);
        }
      }

      util_str_free_split(toks, count);
    }

    return res;
  }

  return pro_usb_fix_real_readlink(path, buf, len);
}

ssize_t readlink(const char *path, char *buf, size_t len)
{
  return propatch_usb_fix_readlink(path, buf, len);
}

void propatch_usb_fix_init(
    const char *device_nodes, const char *p1_bus_port, const char *p2_bus_port)
{
  size_t count;
  char **toks;

  if (!device_nodes || !p1_bus_port || !p2_bus_port) {
    log_warn("Disabled, missing one or multiple option properties");
    return;
  }

  toks = util_str_split(device_nodes, ",", &count);

  if (count != 2) {
    log_error("Invalid format for device nodes: %s", device_nodes);
    util_str_free_split(toks, count);
    return;
  }

  strcpy(propatch_usb_fix_device_thumb_drive_p1, toks[0]);
  strcpy(propatch_usb_fix_device_thumb_drive_p2, toks[1]);

  util_str_free_split(toks, count);

  if (sscanf(
          p1_bus_port,
          "%d-%d",
          &propatch_usb_fix_usb_bus[0],
          &propatch_usb_fix_usb_port[0]) != 2) {
    log_error("Invalid format for p1 bus-port: %s", p1_bus_port);
    return;
  }

  if (sscanf(
          p2_bus_port,
          "%d-%d",
          &propatch_usb_fix_usb_bus[1],
          &propatch_usb_fix_usb_port[1]) != 2) {
    log_error("Invalid format for p2 bus-port: %s", p2_bus_port);
    return;
  }

  log_info(
      "Initialized: device nodes %s,%s, p1 %d-%d, p2 %d-%d",
      propatch_usb_fix_device_thumb_drive_p1,
      propatch_usb_fix_device_thumb_drive_p2,
      propatch_usb_fix_usb_bus[0],
      propatch_usb_fix_usb_port[0],
      propatch_usb_fix_usb_bus[1],
      propatch_usb_fix_usb_port[1]);

  propatch_usb_fix_enabled = true;
}

void propatch_usb_fix_shutdown()
{
}