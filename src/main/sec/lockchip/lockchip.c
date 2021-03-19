#define LOG_MODULE "sec-lockchip"

#include "sec/lockchip/lockchip.h"

static const uint8_t sec_lockchip_initial_sbox[8] = {
    0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x7f};

static const uint8_t *sec_lockchip_transform;
static uint8_t sec_lockchip_state;

static void sec_lockchip_apply_initial_sbox(void)
{
  uint8_t r = 0;

  for (uint8_t i = 0; i < 8; i++) {

    if (sec_lockchip_state & (1 << i)) {
      r ^= sec_lockchip_initial_sbox[i];
    }
  }

  sec_lockchip_state = r;
}

static uint8_t sec_lockchip_comp_sbox_ceof(uint8_t sel, uint8_t bit)
{
  uint8_t r;

  if (!sel) {
    return sec_lockchip_transform[bit];
  }

  r = sec_lockchip_comp_sbox_ceof((sel - 1) & 7, (bit - 1) & 7);
  r = (r << 1) | (((r >> 7) ^ (r >> 6)) & 1);

  if (bit != 7) {
    return r;
  }

  return r ^ sec_lockchip_comp_sbox_ceof(sel, 0);
}

static void sec_lockchip_apply_bit_sbox(uint8_t bit)
{
  uint8_t r = 0;

  for (uint8_t i = 0; i < 8; i++) {

    if (sec_lockchip_state & (1 << i)) {
      r ^= sec_lockchip_comp_sbox_ceof(bit, i);
    }
  }

  sec_lockchip_state = r;
}

void sec_lockchip_init(const uint8_t *transform)
{
  sec_lockchip_transform = transform;
  sec_lockchip_state = 0;
}

void sec_lockchip_start(void)
{
  sec_lockchip_state = 0xFC;
}

uint8_t sec_lockchip_step(uint8_t data)
{
  uint8_t result = 0;

  for (uint8_t bit = 0; bit < 8; bit++) {

    if (bit == 0) {
      sec_lockchip_apply_initial_sbox();
    }

    result ^= (((sec_lockchip_state >> bit) & 1) << bit);

    if (((data >> bit) & 1) == 0) {
      sec_lockchip_apply_bit_sbox(bit);
    }
  }

  return result;
}