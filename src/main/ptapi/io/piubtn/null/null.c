/**
 * Implementation of the piubtn API. Null driver for testing and starting piu without a real io
 */
#define LOG_MODULE "ptapi-io-piubtn-null"

#include "ptapi/io/piubtn.h"

const char* ptapi_io_piubtn_ident(void)
{
    return "null";
}

bool ptapi_io_piubtn_open(void)
{
    return true;
}


void ptapi_io_piubtn_close(void)
{

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

}

void ptapi_io_piubtn_set_output(uint8_t player, const struct ptapi_io_piubtn_outputs* outputs)
{

}