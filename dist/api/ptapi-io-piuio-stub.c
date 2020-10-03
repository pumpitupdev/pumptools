/**
 * Example implementation of the piuio API. This doesn't really implement anything but rather provides a stub file that
 * helps anyone to get started with API development/prototyping.
 *
 * A simple setup for a quick start:
 * 1. Ensure that the API folders with the headers are located next to this file, specifically ptapi/io/piuio.h.
 * 2. Compile using: gcc -I. -m32 -shared -fPIC -o ptapi-io-piuio-stub.so ptapi-io-piuio-stub.c
 * 3. Follow the steps in the readme to configure it for the game of your choice.
 */
#include <stdlib.h>
#include <stdio.h>

#include "ptapi/io/piuio.h"

const char* ptapi_io_piuio_ident(void)
{
    return "stub";
}

bool ptapi_io_piuio_open(void)
{
    printf("piuio-stub: ptapi_io_piuio_open\n");

    return true;
}

void ptapi_io_piuio_close(void)
{
    printf("piuio-stub: ptapi_io_piuio_close\n");
}

bool ptapi_io_piuio_recv(void)
{
    return true;
}

bool ptapi_io_piuio_send(void)
{
    return true;
}

void ptapi_io_piuio_get_input_pad(uint8_t player, enum ptapi_io_piuio_sensor_group sensor_group,
        struct ptapi_io_piuio_pad_inputs* inputs)
{
    // Set all inputs to off
    if (player == 0) {
        inputs->lu = false;
        inputs->ru = false;
        inputs->cn = false;
        inputs->ld = false;
        inputs->rd = false;
    } else {
        inputs->lu = false;
        inputs->ru = false;
        inputs->cn = false;
        inputs->ld = false;
        inputs->rd = false;
    }
}

void ptapi_io_piuio_get_input_sys(struct ptapi_io_piuio_sys_inputs* inputs)
{
    // Set all inputs to off
    inputs->test = false;
    inputs->service = false;
    inputs->clear = false;
    inputs->coin = false;
    inputs->coin2 = false;
}

void ptapi_io_piuio_set_output_pad(uint8_t player, const struct ptapi_io_piuio_pad_outputs* outputs)
{

}

void ptapi_io_piuio_set_output_cab(const struct ptapi_io_piuio_cab_outputs* outputs)
{

}