/**
 * Lockchip (MK3 games) emulator
 *
 * Credits to TheSchaf for the emulation code and transform data
 */
#ifndef SEC_LOCKCHIP_H
#define SEC_LOCKCHIP_H

#include <stdint.h>

/**
 * Initialize (or reset) the lockchip emulation module
 *
 * @param transform Pointer to a transform to use for the step calls
 */
void sec_lockchip_init(const uint8_t *transform);

/**
 * Start lockchip emulation and set the initial state before calling step
 */
void sec_lockchip_start(void);

/**
 * Execute a step on some data
 *
 * @param data Input data to transform
 * @return Transformed output
 */
uint8_t sec_lockchip_step(uint8_t data);

#endif