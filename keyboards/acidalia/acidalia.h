#pragma once

#include "quantum.h"

#define LAYOUT_all( \
    k00, k01, k02, k03, \
    k10, k11, k12, k13, \
    k20, k21, k22, k23, \
         k30, k31 \
    ) { \
        { k00, k01, k02, k03 }, \
        { k10, k11, k12, k13 }, \
        { k20, k21, k22, k23 }, \
        { KC_NO, KC_NO, KC_NO, KC_NO } \
    }

enum custom_keycodes {
    KC_SCROLL = SAFE_RANGE,
    KC_CPI_1,
    KC_CPI_2,
    KC_CPI_3,
    KC_MNT
};

typedef union {
  uint32_t raw;
  struct {
    uint16_t cpi;
  };
} config_acidalia_t;
