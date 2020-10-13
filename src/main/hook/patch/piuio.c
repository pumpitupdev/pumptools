#define LOG_MODULE "patch-piuio"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "capnhook/hooklib/usb-emu.h"

#include "io/piuio/defs.h"

#include "ptapi/io/piuio.h"
#include "ptapi/io/piuio/util/lib.h"

#include "util/log.h"
#include "util/time.h"

static bool _patch_piuio_enumerate(bool real_exists);
static enum cnh_result _patch_piuio_open(void);
static enum cnh_result _patch_piuio_reset(void);
static enum cnh_result _patch_piuio_control_msg(int request_type, int request, int value, int index,
        struct cnh_iobuf* buffer, int timeout);
static void _patch_piuio_close(void);

static enum cnh_result _patch_piuio_process_inputs(struct cnh_iobuf* buffer);
static enum cnh_result _patch_piuio_process_outputs(struct cnh_iobuf* buffer);
static enum cnh_result _patch_piuio_process_itg_kernel_module_hack(struct cnh_iobuf* buffer);

static const struct cnh_usb_emu_virtdev_ep _patch_piuio_virtdev = {
    .pid = PIUIO_DRV_PID,
    .vid = PIUIO_DRV_VID,
    .enumerate = _patch_piuio_enumerate,
    .open = _patch_piuio_open,
    .reset = _patch_piuio_reset,
    .control_msg = _patch_piuio_control_msg,
    .close = _patch_piuio_close,
};

static struct ptapi_io_piuio_api _patch_piuio_api;
static enum ptapi_io_piuio_sensor_group _patch_piuio_sensor_group;

void patch_piuio_init(const char* piuio_lib_path)
{
    if (!piuio_lib_path) {
        log_die("No piuio emulation library path specified");
    }

    /* Load piuio library funcs */
    log_info("Loading piuio api implementation %s", piuio_lib_path);

    if (!ptapi_io_piuio_util_lib_load(piuio_lib_path, &_patch_piuio_api)) {
        log_error("Loading PIUIO API from %s failed", piuio_lib_path);
        return;
    }

    cnh_usb_emu_add_virtdevep(&_patch_piuio_virtdev);

    log_info("Initialized");
}

void patch_piuio_shutdown(void)
{

}

static bool _patch_piuio_enumerate(bool real_exists)
{
    if (real_exists) {
        log_info("Real PIUIO exists but emulation enabled, blocking real");
    } else {
        log_info("Enumerating PIUIO emulation");
    }

    return true;
}

static enum cnh_result _patch_piuio_open(void)
{
    log_info("Opening PIUIO: %s", _patch_piuio_api.ident());

    if (!_patch_piuio_api.open()) {
        log_error("Opening api piuio %s failed", _patch_piuio_api.ident());
        return CNH_RESULT_OTHER_ERROR;
    }

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_piuio_reset(void)
{
    log_info("Resetting PIUIO: %s", _patch_piuio_api.ident());

    _patch_piuio_api.close();

    if (!_patch_piuio_api.open()) {
        log_error("Resetting api piuio %s failed", _patch_piuio_api.ident());
        return CNH_RESULT_OTHER_ERROR;
    }

    return CNH_RESULT_SUCCESS;
}

// TODO cleanup code and hide measuring time behind feature flag, add this to
// all supported games + documentation for debugging purpose to detect lag issues
// (potentially with IO implementations)
static util_time_counter_t _patch_piuio_poll_timer_start;
static uint32_t _patch_piuio_poll_counter;

static enum cnh_result _patch_piuio_control_msg(int request_type, int request, int value, int index,
        struct cnh_iobuf* buffer, int timeout)
{
    enum cnh_result result;
    util_time_counter_t time_counter_start;
    util_time_counter_t time_counter_delta;
    double time_ms;

    if (_patch_piuio_poll_timer_start == 0) {
        _patch_piuio_poll_counter = 0;
        _patch_piuio_poll_timer_start = util_time_get_counter();
    } else if (util_time_get_elapsed_ms_double(util_time_get_counter() - _patch_piuio_poll_timer_start) >= 1000.0) {
          log_debug("ctrl_msg refresh rate: %d hz", _patch_piuio_poll_counter);
          _patch_piuio_poll_counter = 0;
          _patch_piuio_poll_timer_start = util_time_get_counter();
    }

    time_counter_start = util_time_get_counter();

    if (request_type == PIUIO_DRV_USB_CTRL_TYPE_IN && request == PIUIO_DRV_USB_CTRL_REQUEST) {
        if (buffer->nbytes != PIUIO_DRV_BUFFER_SIZE) {
            log_error("Invalid buffer size for ctrl in: %d", buffer->nbytes);
            result = CNH_RESULT_INVALID_PARAMETER;
        } else {
            result = _patch_piuio_process_inputs(buffer);
        }
    } else if (request_type == PIUIO_DRV_USB_CTRL_TYPE_OUT && request == PIUIO_DRV_USB_CTRL_REQUEST) {
        if (buffer->nbytes != PIUIO_DRV_BUFFER_SIZE) {
            log_error("Invalid buffer size for ctrl out: %d", buffer->nbytes);
            result = CNH_RESULT_INVALID_PARAMETER;
        } else {
            result = _patch_piuio_process_outputs(buffer);
        }
    } else if (request_type == 0 && request == 0) {
        /* ITG 2/PIU Pro kernel hack */
        if (buffer->nbytes != PIUIO_DRV_BUFFER_SIZE * 4) {
            log_error("Invalid buffer size for kernel hack: %d", buffer->nbytes);
            result = CNH_RESULT_INVALID_PARAMETER;
        } else {
            result = _patch_piuio_process_itg_kernel_module_hack(buffer);
        }
    } else {
        log_error("Invalid usb control request to PIUIO: 0x%02X", request);
        result = CNH_RESULT_INVALID_PARAMETER;
    }

    time_counter_delta = util_time_get_counter() - time_counter_start;
    time_ms = util_time_get_elapsed_ms_double(time_counter_delta);

    if (time_ms > 0.010) {
        log_debug("High processing time sample: %f ms", time_ms);
    }

    _patch_piuio_poll_counter++;

    return result;
}

static void _patch_piuio_close(void)
{
    log_info("Closing PIUIO: %s", _patch_piuio_api.ident());

    _patch_piuio_api.close();
}

static enum cnh_result _patch_piuio_process_inputs(struct cnh_iobuf* buffer)
{
    struct ptapi_io_piuio_pad_inputs p1_pad_in;
    struct ptapi_io_piuio_pad_inputs p2_pad_in;
    struct ptapi_io_piuio_sys_inputs sys_in;

    if (!_patch_piuio_api.recv()) {
        log_error("Receiving inputs on api piuio %s failed", _patch_piuio_api.ident());
        return CNH_RESULT_OTHER_ERROR;
    }

    memset(&p1_pad_in, 0, sizeof(struct ptapi_io_piuio_pad_inputs));
    memset(&p2_pad_in, 0, sizeof(struct ptapi_io_piuio_pad_inputs));
    memset(&sys_in, 0, sizeof(struct ptapi_io_piuio_sys_inputs));

    _patch_piuio_api.get_input_pad(0, _patch_piuio_sensor_group, &p1_pad_in);
    _patch_piuio_api.get_input_pad(1, _patch_piuio_sensor_group, &p2_pad_in);

    _patch_piuio_api.get_input_sys(&sys_in);

    /*
       byte 0:
	   bit 0: sensor p1: LU
	   bit 1: sensor p1: RU
	   bit 2: sensor p1: CN
	   bit 3: sensor p1: LD
	   bit 4: sensor p1: RD
	   bit 5:
	   bit 6:
	   bit 7:

	   byte 1:
	   bit 0:
	   bit 1: test
	   bit 2: coin 1
	   bit 3:
	   bit 4:
	   bit 5:
	   bit 6: service
	   bit 7: clear

	   byte 2:
	   bit 0: sensor p2: LU
	   bit 1: sensor p2: RU
	   bit 2: sensor p2: CN
	   bit 3: sensor p2: LD
	   bit 4: sensor p2: RD
	   bit 5:
	   bit 6:
	   bit 7:

	   byte 3:
	   bit 0:
	   bit 1:
	   bit 2: coin 2
	   bit 3:
	   bit 4:
	   bit 5:
	   bit 6:
	   bit 7:

	   bytes 4 - 7 dummy
    */

    /* Player 1 */
    buffer->bytes[0] = 0;

    if (p1_pad_in.lu) {
        buffer->bytes[0] |= (1 << 0);
    }

    if (p1_pad_in.ru) {
        buffer->bytes[0] |= (1 << 1);
    }

    if (p1_pad_in.cn) {
        buffer->bytes[0] |= (1 << 2);
    }

    if (p1_pad_in.ld) {
        buffer->bytes[0] |= (1 << 3);
    }

    if (p1_pad_in.rd) {
        buffer->bytes[0] |= (1 << 4);
    }

    buffer->bytes[0] ^= 0xFF;

    buffer->bytes[2] = 0;

    /* Player 2 */
    if (p2_pad_in.lu) {
        buffer->bytes[2] |= (1 << 0);
    }

    if (p2_pad_in.ru) {
        buffer->bytes[2] |= (1 << 1);
    }

    if (p2_pad_in.cn) {
        buffer->bytes[2] |= (1 << 2);
    }

    if (p2_pad_in.ld) {
        buffer->bytes[2] |= (1 << 3);
    }

    if (p2_pad_in.rd) {
        buffer->bytes[2] |= (1 << 4);
    }

    buffer->bytes[2] ^= 0xFF;

    buffer->bytes[1] = 0;

    /* Sys */
    if (sys_in.test) {
        buffer->bytes[1] |= (1 << 1);
    }

    if (sys_in.service) {
        buffer->bytes[1] |= (1 << 6);
    }

    if (sys_in.clear) {
        buffer->bytes[1] |= (1 << 7);
    }

    if (sys_in.coin) {
        buffer->bytes[1] |= (1 << 2);
    }

    buffer->bytes[1] ^= 0xFF;

    /* Apparently, "touching" byte 3 as a whole causes some random and weird input triggering
       which results in the service menu popping up. Don't clear the whole byte, touch coin2 only to avoid this */
    if (!sys_in.coin2) {
        buffer->bytes[3] |= (1 << 2);
    } else {
        buffer->bytes[3] &= ~(1 << 2);
    }

    buffer->pos = PIUIO_DRV_BUFFER_SIZE;

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_piuio_process_outputs(struct cnh_iobuf* buffer)
{
    struct ptapi_io_piuio_pad_outputs p1_pad_out;
    struct ptapi_io_piuio_pad_outputs p2_pad_out;
    struct ptapi_io_piuio_cab_outputs cab_out;

    memset(&p1_pad_out, 0, sizeof(struct ptapi_io_piuio_pad_outputs));
    memset(&p2_pad_out, 0, sizeof(struct ptapi_io_piuio_pad_outputs));
    memset(&cab_out, 0, sizeof(struct ptapi_io_piuio_cab_outputs));

    /*
	   byte 0:
	   bit 0: sensor id bit 0	select sensor to be read for next input request:
	   bit 1: sensor id bit 1   00 = sens1, 01 = sens2, 10 = sens3, 11 = sens4
	   bit 2: pad light p1: LU
	   bit 3: pad light p1: RU
	   bit 4: pad light p1: CN
	   bit 5: pad light p1: LD
	   bit 6: pad light p1: RD
	   bit 7:

	   byte 1:
	   bit 0:
	   bit 1:
	   bit 2: light neons
	   bit 3: coin counter 2 (front usb ports enable)
	   bit 4:
	   bit 5:
	   bit 6:
	   bit 7:

	   byte 2:
	   bit 0:
	   bit 1:
	   bit 2: pad light p2: LU
	   bit 3: pad light p2: RU
	   bit 4: pad light p2: CN
	   bit 5: pad light p2: LD
	   bit 6: pad light p2: RD
	   bit 7: halogen R2

	   byte 3:
	   bit 0: halogen R1
	   bit 1: halogen L2
	   bit 2: halogen L1
	   bit 3:
	   bit 4: coin counter 1
	   bit 5:
	   bit 6:
	   bit 7:

	   bytes 4 - 7 dummy
    */

    _patch_piuio_sensor_group = (enum ptapi_io_piuio_sensor_group) (buffer->bytes[0] & 0x03);

    /* Player 1 */

    if (buffer->bytes[0] & (1 << 2)) {
        p1_pad_out.lu = true;
    }

    if (buffer->bytes[0] & (1 << 3)) {
        p1_pad_out.ru = true;
    }

    if (buffer->bytes[0] & (1 << 4)) {
        p1_pad_out.cn = true;
    }

    if (buffer->bytes[0] & (1 << 5)) {
        p1_pad_out.ld = true;
    }

    if (buffer->bytes[0] & (1 << 6)) {
        p1_pad_out.rd = true;
    }

    /* Player 2 */

    if (buffer->bytes[2] & (1 << 2)) {
        p2_pad_out.lu = true;
    }

    if (buffer->bytes[2] & (1 << 3)) {
        p2_pad_out.ru = true;
    }

    if (buffer->bytes[2] & (1 << 4)) {
        p2_pad_out.cn = true;
    }

    if (buffer->bytes[2] & (1 << 5)) {
        p2_pad_out.ld = true;
    }

    if (buffer->bytes[2] & (1 << 6)) {
        p2_pad_out.rd = true;
    }

    /* Cabinet */

    if (buffer->bytes[1] & (1 << 2)) {
        cab_out.bass = true;
    }

    if (buffer->bytes[2] & (1 << 7)) {
        cab_out.halo_r2 = true;
    }

    if (buffer->bytes[3] & (1 << 0)) {
        cab_out.halo_r1 = true;
    }

    if (buffer->bytes[3] & (1 << 1)) {
        cab_out.halo_l2 = true;
    }

    if (buffer->bytes[3] & (1 << 2)) {
        cab_out.halo_l1 = true;
    }

    _patch_piuio_api.set_output_pad(0, &p1_pad_out);
    _patch_piuio_api.set_output_pad(1, &p2_pad_out);
    _patch_piuio_api.set_output_cab(&cab_out);

    if (!_patch_piuio_api.send()) {
        log_error("Sending outputs on api piuio %s failed", _patch_piuio_api.ident());
        return CNH_RESULT_OTHER_ERROR;
    }

    buffer->pos = PIUIO_DRV_BUFFER_SIZE;

    return CNH_RESULT_SUCCESS;
}

static enum cnh_result _patch_piuio_process_itg_kernel_module_hack(struct cnh_iobuf* buffer)
{
    /* Instead of calling four times, one call for each sensor set, a special
       kernel module is loaded which traps the following call:
       usb_control_msg(dev_handle, 0, 0, 0, 0, buffer, 32, 10011);
       All four sensor sets are polled in a single context switch and returned
       as one big buffer instead of four smalle ones (8 bytes each).
       Pump Pro relies on this, so we have to support it here */

    struct ptapi_io_piuio_pad_inputs p1_pad_in[4];
    struct ptapi_io_piuio_pad_inputs p2_pad_in[4];
    struct ptapi_io_piuio_sys_inputs sys_in;

    /* The output duplicated 4 times (4 * 8) in the buffer. Write it once */
    _patch_piuio_process_outputs(buffer);

    if (!_patch_piuio_api.recv()) {
        log_error("Receiving inputs on api piuio %s failed", _patch_piuio_api.ident());
        return CNH_RESULT_OTHER_ERROR;
    }

    memset(buffer->bytes, 0, PIUIO_DRV_BUFFER_SIZE * 4);

    memset(&sys_in, 0, sizeof(struct ptapi_io_piuio_sys_inputs));

    for (uint8_t i = 0; i < 4; i++) {
        memset(&p1_pad_in[i], 0, sizeof(struct ptapi_io_piuio_pad_inputs));
        memset(&p2_pad_in[i], 0, sizeof(struct ptapi_io_piuio_pad_inputs));

        _patch_piuio_api.get_input_pad(0, (enum ptapi_io_piuio_sensor_group) i, &p1_pad_in[i]);
        _patch_piuio_api.get_input_pad(1, (enum ptapi_io_piuio_sensor_group) i, &p2_pad_in[i]);
    }

    _patch_piuio_api.get_input_sys(&sys_in);

    /*
       byte 0:
	   bit 0: sensor p1: LU
	   bit 1: sensor p1: RU
	   bit 2: sensor p1: CN
	   bit 3: sensor p1: LD
	   bit 4: sensor p1: RD
	   bit 5:
	   bit 6:
	   bit 7:

	   byte 1:
	   bit 0:
	   bit 1: test
	   bit 2: coin 1
	   bit 3:
	   bit 4:
	   bit 5:
	   bit 6: service
	   bit 7: clear

	   byte 2:
	   bit 0: sensor p2: LU
	   bit 1: sensor p2: RU
	   bit 2: sensor p2: CN
	   bit 3: sensor p2: LD
	   bit 4: sensor p2: RD
	   bit 5:
	   bit 6:
	   bit 7:

	   byte 3:
	   bit 0:
	   bit 1:
	   bit 2: coin 2
	   bit 3:
	   bit 4:
	   bit 5:
	   bit 6:
	   bit 7:

	   bytes 4 - 7 dummy
    */

    for (uint8_t i = 0; i < 4; i++) {
        /* Player 1 */
        if (p1_pad_in[i].lu) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 0] |= (1 << 0);
        }

        if (p1_pad_in[i].ru) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 0] |= (1 << 1);
        }

        if (p1_pad_in[i].cn) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 0] |= (1 << 2);
        }

        if (p1_pad_in[i].ld) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 0] |= (1 << 3);
        }

        if (p1_pad_in[i].rd) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 0] |= (1 << 4);
        }

        /* Player 2 */
        if (p2_pad_in[i].lu) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 2] |= (1 << 0);
        }

        if (p2_pad_in[i].ru) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 2] |= (1 << 1);
        }

        if (p2_pad_in[i].cn) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 2] |= (1 << 2);
        }

        if (p2_pad_in[i].ld) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 2] |= (1 << 3);
        }

        if (p2_pad_in[i].rd) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 2] |= (1 << 4);
        }

        /* Sys */
        if (sys_in.test) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 1] |= (1 << 1);
        }

        if (sys_in.service) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 1] |= (1 << 6);
        }

        if (sys_in.clear) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 1] |= (1 << 7);
        }

        if (sys_in.coin) {
            buffer->bytes[PIUIO_DRV_BUFFER_SIZE * i + 1] |= (1 << 2);
        }
    }

    /* xor inputs because pullup active */
    for (int i = 0; i < PIUIO_DRV_BUFFER_SIZE * 4; i++) {
        buffer->bytes[i] ^= 0xFF;
    }

    buffer->nbytes = PIUIO_DRV_BUFFER_SIZE * 4;

    return CNH_RESULT_SUCCESS;
}