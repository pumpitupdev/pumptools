/**
 * Example implementation of the piubtn API. This doesn't really implement anything but rather provides a stub file that
 * helps anyone to get started with API development/prototyping.
 *
 * A simple setup for a quick start:
 * 1. Ensure that the API folders with the headers are located next to this file, specifically ptapi/io/piubtn.h.
 * 2. Compile using: gcc -I. -m32 -shared -fPIC -o ptapi-io-piubtn-stub.so ptapi-io-piubtn-stub.c
 * 3. Follow the steps in the readme to configure it for the game of your choice.
 */
#include <stdlib.h>
#include <stdio.h>

#include "ptapi/io/piubtn.h"

const char* ptapi_io_piubtn_ident(void)
{
    return "stub";
}

bool ptapi_io_piubtn_open(void)
{
    printf("piubtn-stub: ptapi_io_piubtn_open\n");

    return true;
}


void ptapi_io_piubtn_close(void)
{
    printf("piubtn-stub: ptapi_io_piubtn_close\n");
}

bool ptapi_io_piubtn_recv(void)
{
    return true;
}

bool ptapi_io_piubtn_send(void)
{
    return true;
}

void ptapi_io_piubtn_get_input(uint8_t player, struct ptapi_io_piubtn_inputs* inputs)
{
    // Set all inputs to off
    if (player == 0) {
        inputs->left = false;
        inputs->right = false;
        inputs->start = false;
        inputs->back = false;
    } else {
        inputs->left = false;
        inputs->right = false;
        inputs->start = false;
        inputs->back = false;
    }
}

void ptapi_io_piubtn_set_output(uint8_t player, const struct ptapi_io_piubtn_outputs* outputs)
{

}