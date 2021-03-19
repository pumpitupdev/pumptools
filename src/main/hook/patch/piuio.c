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

// Enable this to get a detailed "call trace" of reads/writes and updates
// for debugging purpose
// #define PATCH_PIUIO_CALL_TRACE

static bool _patch_piuio_enumerate(bool real_exists);
static enum cnh_result _patch_piuio_open(void);
static enum cnh_result _patch_piuio_reset(void);
static enum cnh_result _patch_piuio_control_msg(int request_type, int request, int value, int index,
        struct cnh_iobuf* buffer, int timeout);
static void _patch_piuio_close(void);

static void _patch_piuio_read_inputs_to_buffer(struct cnh_iobuf* buffer);
static void _patch_piuio_read_outputs_from_buffer(struct cnh_iobuf* buffer);
static enum ptapi_io_piuio_sensor_group _patch_piuio_get_sensor_group_from_buffer(struct cnh_iobuf* buffer);

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

static enum cnh_result _patch_piuio_control_msg(int request_type, int request, int value, int index,
        struct cnh_iobuf* buffer, int timeout)
{
    /**
     * Expected call pattern for a full game state update on a single frame (when done synchronously)
     *
     * CTRL_OUT: light data + sensor 0
     * CTRL_IN: input data of sensor 0
     * CTRL_OUT: light data + sensor 1
     * CTRL_IN: input data of sensor 1
     * CTRL_OUT: light data + sensor 2
     * CTRL_IN: input data of sensor 2
     * CTRL_OUT: light data + sensor 3
     * CTRL_IN: input data of sensor 3
     *
     * Reduce call overhead to pumptools's piuio API for recv and send polling any potential implementation
     */
    if (request_type == PIUIO_DRV_USB_CTRL_TYPE_IN && request == PIUIO_DRV_USB_CTRL_REQUEST) {
        if (buffer->nbytes != PIUIO_DRV_BUFFER_SIZE) {
            log_error("Invalid buffer size for ctrl in: %d", buffer->nbytes);
            return CNH_RESULT_INVALID_PARAMETER;
        }

#ifdef PATCH_PIUIO_CALL_TRACE
        log_debug("Read");
#endif

        // Only read buffered inputs, no need to trigger another update since
        // this is taken of by the cycle start

        _patch_piuio_read_inputs_to_buffer(buffer);

        return CNH_RESULT_SUCCESS;
    } else if (request_type == PIUIO_DRV_USB_CTRL_TYPE_OUT && request == PIUIO_DRV_USB_CTRL_REQUEST) {
        if (buffer->nbytes != PIUIO_DRV_BUFFER_SIZE) {
            log_error("Invalid buffer size for ctrl out: %d", buffer->nbytes);
            return CNH_RESULT_INVALID_PARAMETER;
        }

        _patch_piuio_read_outputs_from_buffer(buffer);

        // Sync properly with sensor cycling by application
        // Note: Naturally, the update code below will break if sensors are not cycled
        // as expected
        _patch_piuio_sensor_group = _patch_piuio_get_sensor_group_from_buffer(buffer);

#ifdef PATCH_PIUIO_CALL_TRACE
        log_debug("Write: %d", _patch_piuio_sensor_group);
#endif

        // Trigger exactly ONE full update cycle on the API implementation on
        // every first call of the whole update cycle

        if (_patch_piuio_sensor_group == 0) {
#ifdef PATCH_PIUIO_CALL_TRACE
            log_debug("Update API");
#endif

            if (!_patch_piuio_api.send()) {
                log_error("Sending outputs on api piuio %s failed", _patch_piuio_api.ident());
                return CNH_RESULT_OTHER_ERROR;
            }

            if (!_patch_piuio_api.recv()) {
                log_error("Receiving inputs on api piuio %s failed", _patch_piuio_api.ident());
                return CNH_RESULT_OTHER_ERROR;
            }
        }

        return CNH_RESULT_SUCCESS;
    } else if (request_type == 0 && request == 0) {
        // ITG 2/PIU Pro kernel hack, can be handled by piuio-khack module
        // Safety net for visibility if the module is missing
        log_error("Unhandled PIUIO kernel hack request detected, cannot dispatch");

        return CNH_RESULT_INVALID_PARAMETER;
    } else {
        log_error("Invalid usb control request to PIUIO: 0x%02X", request);
        return CNH_RESULT_INVALID_PARAMETER;
    }
}

static void _patch_piuio_close(void)
{
    log_info("Closing PIUIO: %s", _patch_piuio_api.ident());

    _patch_piuio_api.close();
}

static void _patch_piuio_read_inputs_to_buffer(struct cnh_iobuf* buffer)
{
    struct ptapi_io_piuio_pad_inputs p1_pad_in;
    struct ptapi_io_piuio_pad_inputs p2_pad_in;
    struct ptapi_io_piuio_sys_inputs sys_in;

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

    buffer->bytes[0] |= ((p1_pad_in.lu ? 1 : 0) << 0);
    buffer->bytes[0] |= ((p1_pad_in.ru ? 1 : 0) << 1);
    buffer->bytes[0] |= ((p1_pad_in.cn ? 1 : 0) << 2);
    buffer->bytes[0] |= ((p1_pad_in.ld ? 1 : 0) << 3);
    buffer->bytes[0] |= ((p1_pad_in.rd ? 1 : 0) << 4);

    buffer->bytes[0] ^= 0xFF;

    /* Player 2 */
    buffer->bytes[2] = 0;

    buffer->bytes[2] |= ((p1_pad_in.lu ? 1 : 0) << 0);
    buffer->bytes[2] |= ((p1_pad_in.ru ? 1 : 0) << 1);
    buffer->bytes[2] |= ((p1_pad_in.cn ? 1 : 0) << 2);
    buffer->bytes[2] |= ((p1_pad_in.ld ? 1 : 0) << 3);
    buffer->bytes[2] |= ((p1_pad_in.rd ? 1 : 0) << 4);

    buffer->bytes[2] ^= 0xFF;

    /* Sys */
    buffer->bytes[1] = 0;

    buffer->bytes[1] |= ((sys_in.test ? 1 : 0) << 1);
    buffer->bytes[1] |= ((sys_in.service ? 1 : 0) << 6);
    buffer->bytes[1] |= ((sys_in.clear ? 1 : 0) << 7);
    buffer->bytes[1] |= ((sys_in.coin ? 1 : 0) << 2);

    buffer->bytes[1] ^= 0xFF;

    /* Apparently, "touching" byte 3 as a whole causes some random and weird input triggering
       which results in the service menu popping up. Don't clear the whole byte, touch coin2 only to avoid this */
    if (!sys_in.coin2) {
        buffer->bytes[3] |= (1 << 2);
    } else {
        buffer->bytes[3] &= ~(1 << 2);
    }

    buffer->pos = PIUIO_DRV_BUFFER_SIZE;
}

static void _patch_piuio_read_outputs_from_buffer(struct cnh_iobuf* buffer)
{
    struct ptapi_io_piuio_pad_outputs p1_pad_out;
    struct ptapi_io_piuio_pad_outputs p2_pad_out;
    struct ptapi_io_piuio_cab_outputs cab_out;

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

    p1_pad_out.lu = (buffer->bytes[0] & (1 << 2)) > 0;
    p1_pad_out.ru = (buffer->bytes[0] & (1 << 3)) > 0;
    p1_pad_out.cn = (buffer->bytes[0] & (1 << 4)) > 0;
    p1_pad_out.ld = (buffer->bytes[0] & (1 << 5)) > 0;
    p1_pad_out.rd = (buffer->bytes[0] & (1 << 6)) > 0;

    p2_pad_out.lu = (buffer->bytes[2] & (1 << 2)) > 0;
    p2_pad_out.ru = (buffer->bytes[2] & (1 << 3)) > 0;
    p2_pad_out.cn = (buffer->bytes[2] & (1 << 4)) > 0;
    p2_pad_out.ld = (buffer->bytes[2] & (1 << 5)) > 0;
    p2_pad_out.rd = (buffer->bytes[2] & (1 << 6)) > 0;

    cab_out.bass = (buffer->bytes[1] & (1 << 2)) > 0;
    cab_out.halo_r2 = (buffer->bytes[2] & (1 << 7)) > 0;
    cab_out.halo_r1 = (buffer->bytes[3] & (1 << 0)) > 0;
    cab_out.halo_l2 = (buffer->bytes[3] & (1 << 1)) > 0;
    cab_out.halo_l1 = (buffer->bytes[3] & (1 << 2)) > 0;

    _patch_piuio_api.set_output_pad(0, &p1_pad_out);
    _patch_piuio_api.set_output_pad(1, &p2_pad_out);
    _patch_piuio_api.set_output_cab(&cab_out);
}

static enum ptapi_io_piuio_sensor_group _patch_piuio_get_sensor_group_from_buffer(struct cnh_iobuf* buffer)
{
    return (enum ptapi_io_piuio_sensor_group) (buffer->bytes[0] & 0x03);
}