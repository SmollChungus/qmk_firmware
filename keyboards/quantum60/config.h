/* Copyright 2022 SmollChungus
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
#define VENDOR_ID       0x5363 // Sc Smollchungus
#define PRODUCT_ID      0x5136 // Q6
#define DEVICE_VER      0x0001
#define MANUFACTURER    smollchungus
#define PRODUCT         smollchungus Quantum60


/* Matrix  */
#define MATRIX_ROWS 5
#define MATRIX_COLS 16

#define MATRIX_ROW_PINS { D5, D3, D2, F4, E6 }
#define MATRIX_COL_PINS { F7, F6, F5, F1, F0, B0, D0, D1, D4, D6, D7, B4, B5, B6, C6, C7 }
#define UNUSED_PINS

/* Diode Orientation*/
#define DIODE_DIRECTION COL2ROW


/* Debounce*/
#define DEBOUNCE 5

/* Locking support */
#define LOCKING_SUPPORT_ENABLE
#define LOCKING_RESYNC_ENABLE

