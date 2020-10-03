#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <linux/joystick.h>
#include <sys/ioctl.h>

#include "io/util/joystick.h"

#include "util/log.h"
#include "util/mem.h"

#define MAX_NUM_EVENTS 64

bool io_util_joystick_open(const char* dev_path, struct io_util_joystick** ctx)
{
    log_assert(dev_path);
    log_assert(ctx);

    int handle;

    handle = open(dev_path, O_RDONLY | O_NONBLOCK);

    if (handle == -1) {
        log_error("Opening joystick device %s failed: %s", dev_path, strerror(errno));
        return false;
    }

    *ctx = util_xmalloc(sizeof(struct io_util_joystick));

    strcpy((*ctx)->dev_path, dev_path);
    (*ctx)->handle = handle;

    // Init values to avoid having ioctl return garbage
    (*ctx)->num_buttons = 0;
    (*ctx)->num_axes = 0;
    memset((*ctx)->name, 0, sizeof(char) * JOYSTICK_NAME_LEN);

    // grab data
    ioctl(handle, JSIOCGAXES, &((*ctx)->num_axes));
    ioctl(handle, JSIOCGBUTTONS, &((*ctx)->num_buttons));
    ioctl(handle, JSIOCGNAME(JOYSTICK_NAME_LEN), (*ctx)->name);

    (*ctx)->button_state = util_xmalloc(sizeof(uint8_t) * (*ctx)->num_buttons);

    log_debug("Opened joystick device %s, handle %p, num buttons %d, num axes %d, name %s", (*ctx)->dev_path,
            (*ctx)->handle, (*ctx)->num_buttons, (*ctx)->num_axes, (*ctx)->name);

    return true;
}

bool io_util_joystick_close(struct io_util_joystick* ctx)
{
    log_assert(ctx);

    bool res;

    log_debug("Closing joystick device %s, handle %p", ctx->dev_path, ctx->handle);

    if (close(ctx->handle) != 0) {
        log_error("Closing joystick device %s failed: %s", ctx->dev_path, strerror(errno));
        res = false;
    } else {
        res = true;
    }

    free(ctx->button_state);
    free(ctx);

    return res;
}

bool io_util_joystick_update(struct io_util_joystick* ctx)
{
    log_assert(ctx);

    struct js_event jse[MAX_NUM_EVENTS];

    int bytes = read(ctx->handle, jse, sizeof(struct js_event) * MAX_NUM_EVENTS);

    if (bytes < 0) {
        switch (errno) {
            // try again on next poll
            case EAGAIN:
                return true;
            default:
                log_error("Reading from joystick device failed: %s", ctx->dev_path, strerror(errno));
                return false;
        }
    }

    // determine how many whole events we read
    bytes /= sizeof(struct js_event);

    // now iterate all events
    for (int i = 0; i < bytes; i++) {
        switch (jse[i].type & ~JS_EVENT_INIT) {
            case JS_EVENT_BUTTON:
                ctx->button_state[jse[i].number] = (bool) jse[i].value;
                break;

            default:
                break;
        }
    }


    return true;
}

bool io_util_joystick_is_button_pressed(struct io_util_joystick* ctx, uint16_t button)
{
    log_assert(ctx);

    if (button >= ctx->num_buttons) {
        return false;
    }

    return ctx->button_state[button];
}