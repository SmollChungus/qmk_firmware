/*
Copyright 2022 smollchungus

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "config_common.h"

/* USB Device descriptor parameter s*/
#define VENDOR_ID       0x5363  // Sc  for Smollchungus
#define PRODUCT_ID      0x0070  // 0070 for Midi70
#define DEVICE_VER      0x0001
#define MANUFACTURER    smollchungus
#define PRODUCT         Midi70


/* key matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 14

#define BOOTMAGIC_LITE_ROW 0
#define BOOTMAGIC_LITE_COLUMN 1

/* Keyboard Matrix Assignments */
#define MATRIX_ROW_PINS { F1, F0, F7, F6, F5, F4  }
#define MATRIX_COL_PINS { C7, C6, B6, B5, B4, D7, D6, D4, D5, D3, D2, D1, D0, B7 }
/* COL2ROW, ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

#undef TAPPING_TERM
#define TAPPING_TERM 190

#define MIDI_ADVANCED

