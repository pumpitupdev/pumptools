/**
 * Implementation of the piubtn API. This implements a piubtn usb driver to
 * communicate with real piubtn hardware and also shares a common interface with
 * other "piubtn devices" (e.g. keyboard, custom IOs)
 */
#define LOG_MODULE "ptapi-io-piubtn-real"

#include <stdlib.h>
#include <string.h>

#include "io/piubtn/defs.h"
#include "io/piubtn/device.h"

#include "ptapi/io/piubtn.h"

static uint8_t piubtn_drv_piubtn_in_buffer[PIUBTN_DRV_BUFFER_SIZE];
static uint8_t piubtn_drv_piubtn_out_buffer[PIUBTN_DRV_BUFFER_SIZE];

static struct ptapi_io_piubtn_inputs piubtn_drv_piubtn_in[2];
static struct ptapi_io_piubtn_outputs piubtn_drv_piubtn_out[2];

static void piubtn_drv_piubtn_copy_outputs()
{
    memset(piubtn_drv_piubtn_out_buffer, 0,
        sizeof(piubtn_drv_piubtn_out_buffer));

    /* Player 1 */

    if (piubtn_drv_piubtn_out[0].start) {
        piubtn_drv_piubtn_out_buffer[0] |= (1 << 0);
    }

    if (piubtn_drv_piubtn_out[0].right) {
        piubtn_drv_piubtn_out_buffer[0] |= (1 << 1);
    }

    if (piubtn_drv_piubtn_out[0].left) {
        piubtn_drv_piubtn_out_buffer[0] |= (1 << 2);
    }

    if (piubtn_drv_piubtn_out[0].back) {
        piubtn_drv_piubtn_out_buffer[0] |= (1 << 3);
    }

    /* Player 2 */

    if (piubtn_drv_piubtn_out[1].start) {
        piubtn_drv_piubtn_out_buffer[0] |= (1 << 4);
    }

    if (piubtn_drv_piubtn_out[1].right) {
        piubtn_drv_piubtn_out_buffer[0] |= (1 << 5);
    }

    if (piubtn_drv_piubtn_out[1].left) {
        piubtn_drv_piubtn_out_buffer[0] |= (1 << 6);
    }

    if (piubtn_drv_piubtn_out[1].back) {
        piubtn_drv_piubtn_out_buffer[0] |= (1 << 7);
    }
}

static void piubtn_drv_piubtn_copy_inputs()
{
    /* Player 1 */
    piubtn_drv_piubtn_in[0].back = piubtn_drv_piubtn_in_buffer[0] & (1 << 0);
    piubtn_drv_piubtn_in[0].left = piubtn_drv_piubtn_in_buffer[0] & (1 << 1);
    piubtn_drv_piubtn_in[0].right = piubtn_drv_piubtn_in_buffer[0] & (1 << 2);
    piubtn_drv_piubtn_in[0].start = piubtn_drv_piubtn_in_buffer[0] & (1 << 3);

    /* Player 2 */
    piubtn_drv_piubtn_in[1].back = piubtn_drv_piubtn_in_buffer[0] & (1 << 4);
    piubtn_drv_piubtn_in[1].left = piubtn_drv_piubtn_in_buffer[0] & (1 << 5);
    piubtn_drv_piubtn_in[1].right = piubtn_drv_piubtn_in_buffer[0] & (1 << 6);
    piubtn_drv_piubtn_in[1].start = piubtn_drv_piubtn_in_buffer[0] & (1 << 7);
}

const char* ptapi_io_piubtn_ident(void)
{
    return "piubtn";
}

bool ptapi_io_piubtn_open(void)
{
    return piubtn_drv_device_open();
}

void ptapi_io_piubtn_close(void)
{
    piubtn_drv_device_close();
}

bool ptapi_io_piubtn_recv(void)
{
    if (!piubtn_drv_device_read(piubtn_drv_piubtn_in_buffer,
            sizeof(piubtn_drv_piubtn_in_buffer))) {
        return false;
    }

    /* Invert pull ups */
    for (uint8_t j = 0; j < 4; j++) {
        piubtn_drv_piubtn_in_buffer[j] ^= 0xFF;
    }

    piubtn_drv_piubtn_copy_inputs();

    return true;
}

bool ptapi_io_piubtn_send(void)
{
    piubtn_drv_piubtn_copy_outputs();

    if (!piubtn_drv_device_write(piubtn_drv_piubtn_out_buffer,
            sizeof(piubtn_drv_piubtn_out_buffer))) {
        return false;
    }

    return true;
}

void ptapi_io_piubtn_get_input(uint8_t player,
        struct ptapi_io_piubtn_inputs* inputs)
{
    memcpy(inputs, &piubtn_drv_piubtn_in[player],
        sizeof(struct ptapi_io_piubtn_inputs));
}

void ptapi_io_piubtn_set_output(uint8_t player,
        const struct ptapi_io_piubtn_outputs* outputs)
{
    memcpy(&piubtn_drv_piubtn_out[player], outputs,
        sizeof(struct ptapi_io_piubtn_outputs));
}