#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <linux/joystick.h>
#include <sys/ioctl.h>

#include "io/util/joystick-util.h"

#include "util/fs.h"
#include "util/log.h"

size_t io_util_joystick_util_scan(struct io_util_joystick_util_device_info* devices, size_t len)
{
    log_assert(devices);

    char dev_path[PATH_MAX];
    int handle;
    size_t num_joysticks;

    num_joysticks = 0;

    for (size_t i = 0; i < len; i++) {
        sprintf(dev_path, "/dev/input/js%d", i);

        memset(&devices[num_joysticks], 0, sizeof(struct io_util_joystick_util_device_info));

        if (util_fs_path_exists(dev_path)) {
            // temporarily have to open the device
            handle = open(dev_path, O_RDONLY | O_NONBLOCK);

            if (handle == -1) {
                log_error("Opening joystick device %s failed: %s", dev_path, strerror(errno));
                continue;
            }

            strcpy(devices[num_joysticks].dev_path, dev_path);

            ioctl(handle, JSIOCGAXES, &(devices[num_joysticks].num_axes));
            ioctl(handle, JSIOCGBUTTONS, &(devices[num_joysticks].num_buttons));
            ioctl(handle, JSIOCGNAME(JOYSTICK_NAME_LEN), devices[num_joysticks].name);

            close(handle);

            num_joysticks++;
        }
    }

    return num_joysticks;
}