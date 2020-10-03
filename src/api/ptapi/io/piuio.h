/**
 * Common API to use for any devices implementing (MK6) piuio controls
 * (e.g. keyboard emulation, custom IOs)
 */
#ifndef PTAPI_IO_PIUIO_H
#define PTAPI_IO_PIUIO_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Enum for pad input sensor groups (four sensors per button/panel)
 */
enum ptapi_io_piuio_sensor_group {
    PTAPI_IO_PIUIO_SENSOR_GROUP_RIGHT = 0,
    PTAPI_IO_PIUIO_SENSOR_GROUP_LEFT = 1,
    PTAPI_IO_PIUIO_SENSOR_GROUP_DOWN = 2,
    PTAPI_IO_PIUIO_SENSOR_GROUP_UP = 3,
    PTAPI_IO_PIUIO_SENSOR_GROUP_NUM = 4,
};

/**
 * Inputs of all arrows of a single pad (per sensor group)
 */
struct ptapi_io_piuio_pad_inputs {
    bool lu;
    bool ru;
    bool cn;
    bool ld;
    bool rd;
};

/**
 * System/operator inputs
 */
struct ptapi_io_piuio_sys_inputs {
    bool test;
    bool service;
    bool clear;
    bool coin;
    bool coin2;
};

/**
 * Pad light outputs
 */
struct ptapi_io_piuio_pad_outputs {
    bool lu;
    bool ru;
    bool cn;
    bool ld;
    bool rd;
};

/**
 * Cabinet light outputs
 */
struct ptapi_io_piuio_cab_outputs {
    bool bass;
    bool halo_r2;
    bool halo_r1;
    bool halo_l2;
    bool halo_l1;
};

typedef const char* (*ptapi_io_piuio_ident_t)(void);
typedef bool (*ptapi_io_piuio_open_t)(void);
typedef void (*ptapi_io_piuio_close_t)(void);
typedef bool (*ptapi_io_piuio_recv_t)(void);
typedef bool (*ptapi_io_piuio_send_t)(void);
typedef void (*ptapi_io_piuio_get_input_pad_t)(uint8_t player,  enum ptapi_io_piuio_sensor_group sensor_group,
    struct ptapi_io_piuio_pad_inputs* inputs);
typedef void (*ptapi_io_piuio_get_input_sys_t)(struct ptapi_io_piuio_sys_inputs* inputs);
typedef void (*ptapi_io_piuio_set_output_pad_t)(uint8_t player, struct ptapi_io_piuio_pad_outputs* outputs);
typedef void (*ptapi_io_piuio_set_output_cab_t)(const struct ptapi_io_piuio_cab_outputs* outputs);

/**
 * API functions in a struct to make them easier to handle and have less boilerplate
 */
struct ptapi_io_piuio_api {
    ptapi_io_piuio_ident_t ident;
    ptapi_io_piuio_open_t open;
    ptapi_io_piuio_close_t close;
    ptapi_io_piuio_recv_t recv;
    ptapi_io_piuio_send_t send;
    ptapi_io_piuio_get_input_pad_t get_input_pad;
    ptapi_io_piuio_get_input_sys_t get_input_sys;
    ptapi_io_piuio_set_output_pad_t set_output_pad;
    ptapi_io_piuio_set_output_cab_t set_output_cab;
};

/**
 * Return an identifier of the implementation. Used for logging and debugging.
 *
 * @return Identifier string for your implementation (e.g. "my-custom-io")
 */
const char* ptapi_io_piuio_ident(void);

/**
 * Open the piuio device
 *
 * Open any file handles, spawn threads and/or initialize your device or emulation code to be ready to accept further
 * commands.
 *
 * @return True if opening/initializing your implementation was sucessful, false on failure
 */
bool ptapi_io_piuio_open(void);

/**
 * Close and cleanup your device, shut down threads, free resources etc
 */
void ptapi_io_piuio_close(void);

/**
 * Get _ALL_ readable data from your device in this single recv call. If you have to address single sensores like the
 * real piuio, you have to do this on a single call to this function.
 *
 * @return True on success, false on error
 */
bool ptapi_io_piuio_recv(void);

/**
 * Send any output data (e.g. lights) to your device
 *
 * @return True on success, false on error
 */
bool ptapi_io_piuio_send(void);

/**
 * Get buffered pad input data
 *
 * @param player Player id (0, 1)
 * @param sensor_group Sensor group states to get
 * @param inputs Pointer to buffer to copy inputs of the selected sensor group to. Every time this is invoked, this
 *               buffer is nulled. If you do not write any data to this, all inputs remain un-triggered.
 */
void ptapi_io_piuio_get_input_pad(uint8_t player, enum ptapi_io_piuio_sensor_group sensor_group,
        struct ptapi_io_piuio_pad_inputs* inputs);

/**
 * Get buffered system (test, service, clear, coin) inputs
 *
 * @param inputs Pointer to buffer to copy inputs to. Every time this is invoked, this buffer is nulled. If you do not
 *               write any data to this, all inputs remain un-triggered.
 */
void ptapi_io_piuio_get_input_sys(struct ptapi_io_piuio_sys_inputs* inputs);

/**
 * Set pad light outputs (i.e. buffer them)
 *
 * @param player Player id (0, 1)
 * @param outputs Pointer to output data to copy to your private buffers.
 */
void ptapi_io_piuio_set_output_pad(uint8_t player, const struct ptapi_io_piuio_pad_outputs* outputs);

/**
 * Set cabinet light outputs (i.e. buffer them)
 *
 * @param outputs Pointer to output data to copy to your private buffers.
 */
void ptapi_io_piuio_set_output_cab(const struct ptapi_io_piuio_cab_outputs* outputs);

#endif