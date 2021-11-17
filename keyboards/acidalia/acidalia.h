/* Copyright 2021 Smoll Chungus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "quantum.h"


// keymap
#define LAYOUT_all( \
    K00,   K01,   K02, K03, \
    K10,   K11,   K12, K13, \
    K20,   K21,   K22, K23, \
           K31,   K32       \
) \
{ \
    { K00,   K01, K02,   K03 }, \
    { K10,   K11, K12,   K13 }, \
    { K20,   K21, K22,   K23 }, \
    { KC_NO, K31, K32,   KC_NO } \
}

#define LAYOUT LAYOUT

enum custom_keycodes {
    KC_SCROLL = SAFE_RANGE,
    KC_CPI_1,
    KC_CPI_2,
    KC_CPI_3
};

typedef union {
  uint32_t raw;
  struct {
    uint16_t cpi;
  };
} config_acidalia_t;
