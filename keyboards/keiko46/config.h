/* Copyright 2021 Smoll Chungus (@smollchungus)
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

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID       0x5363 // Sc for Smollchungus
#define PRODUCT_ID      0x4646
#define DEVICE_VER      0x0001
#define MANUFACTURER    SmollChungus
#define PRODUCT         keiko46

/* key matrix size */
#define MATRIX_ROWS 8
#define MATRIX_COLS 7

#pragma once

/* key matrix pins */
#define MATRIX_ROW_PINS { F4, F5, F6, F7, B1, B3, B2, B6 }
#define MATRIX_COL_PINS { D0, D4, C6, D7, E6, B4, B5  }

/* key matrix pins are in subfolders v1/v2 */

/* Diode direction   */
#define DIODE_DIRECTION COL2ROW

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE

/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define ENCODERS_PAD_A { D1 }
#define ENCODERS_PAD_B { D2 }

/*
#define ENCODER_DIRECTION_FLIP
#define ENCODER_RESOLUTION 4
*/

/* RGB */
#define RGB_DI_PIN D3

#define RGBLED_NUM 12

#define RGBLIGHT_LED_MAP { 0, 1, 2, 3, 4, 5, 11, 10, 9, 8, 7, 6 }
