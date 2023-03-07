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



/* key matrix size */
#define MATRIX_ROWS 2
#define MATRIX_COLS 2

#define DIRECT_PINS { \
    { C5, C4}, \
    { D5 } \
}


// Crunches firmware to support VIA, disable lines if not intending to use VIA

#define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT
#define IGNORE_MOD_TAP_INTERRUPT

#define TAPPING_FORCE_HOLD
