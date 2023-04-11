/*
Copyright 2023 Matthijs Muller

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

/* USB Device descriptor parameter */
#define VENDOR_ID       0x5363 /* "Sc" */
#define PRODUCT_ID      0x4370 /* "Cp" */
#define DEVICE_VER      0x0001
#define MANUFACTURER    Smoll
#define PRODUCT         cosplay_tkl

/* key matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 17

#define MATRIX_ROW_PINS { 8, 11, 10, 9, 13, 12 }
#define MATRIX_COL_PINS { 4, 3, 2, 26, 25, 24, 23, 20, 19, 18, 29, 28, 27, 14, 1, 5, 6 }

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* Use 1000hz polling */
#define USB_POLLING_INTERVAL_MS 1
