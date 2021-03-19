/**
 * Common API to use for any devices implementing piu (pro) button controls
 * (e.g. keyboard emulation, custom IOs)
 */
#ifndef PTAPI_IO_PIUBTN_H
#define PTAPI_IO_PIUBTN_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Button inputs of a single player
 */
struct ptapi_io_piubtn_inputs {
  bool start;
  bool back;
  bool left;
  bool right;
};

/**
 * Button light outputs of a single player
 */
struct ptapi_io_piubtn_outputs {
  bool start;
  bool back;
  bool left;
  bool right;
};

typedef const char *(*ptapi_io_piubtn_ident_t)(void);
typedef bool (*ptapi_io_piubtn_open_t)(void);
typedef void (*ptapi_io_piubtn_close_t)(void);
typedef bool (*ptapi_io_piubtn_recv_t)(void);
typedef bool (*ptapi_io_piubtn_send_t)(void);
typedef void (*ptapi_io_piubtn_get_input_t)(
    uint8_t player, struct ptapi_io_piubtn_inputs *inputs);
typedef void (*ptapi_io_piubtn_set_output_t)(
    uint8_t player, const struct ptapi_io_piubtn_outputs *outputs);

/**
 * API functions in a struct to make them easier to handle and have less
 * boilerplate
 */
struct ptapi_io_piubtn_api {
  ptapi_io_piubtn_ident_t ident;
  ptapi_io_piubtn_open_t open;
  ptapi_io_piubtn_close_t close;
  ptapi_io_piubtn_recv_t recv;
  ptapi_io_piubtn_send_t send;
  ptapi_io_piubtn_get_input_t get_input;
  ptapi_io_piubtn_set_output_t set_output;
};

/**
 * Return an identifier of the implementation. Used for logging and debugging.
 *
 * @return Identifier string for your implementation (e.g. "my-custom-btnio")
 */
const char *ptapi_io_piubtn_ident(void);

/**
 * Open the piubtn device
 *
 * Open any file handles, spawn threads and/or initialize your device or
 * emulation code to be ready to accept further commands.
 *
 * @return True if opening/initializing your implementation was sucessful, false
 * on failure
 */
bool ptapi_io_piubtn_open(void);

/**
 * Close and cleanup your device, shut down threads, free resources etc
 */
void ptapi_io_piubtn_close(void);

/**
 * Get _ALL_ readable data from your device in this single recv call.
 *
 * @return True on success, false on error
 */
bool ptapi_io_piubtn_recv(void);

/**
 * Send any output data (e.g. lights) to your device
 *
 * @return True on success, false on error
 */
bool ptapi_io_piubtn_send(void);

/**
 * Get buffered button input data
 *
 * @param player Player id (0, 1)
 * @param inputs Pointer to buffer to copy inputs to. Every time this is
 * invoked, this buffer is nulled. If you do not write any data to this, all
 * inputs remain un-triggered.
 */
void ptapi_io_piubtn_get_input(
    uint8_t player, struct ptapi_io_piubtn_inputs *inputs);

/**
 * Set button light outputs (i.e. buffer them)
 *
 * @param player Player id (0, 1)
 * @param outputs Pointer to output data to copy to your private buffers.
 */
void ptapi_io_piubtn_set_output(
    uint8_t player, const struct ptapi_io_piubtn_outputs *outputs);

#endif