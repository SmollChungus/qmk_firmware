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

#include QMK_KEYBOARD_H
#include <qp.h>

enum layers {
    _QWERTY,
    _LOWER,
    _RAISE,
    _SPECIAL,
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define SPECIAL MO(_SPECIAL)



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_QWERTY] = LAYOUT_all(
		KC_A
),

[_LOWER] = LAYOUT_all(
		_______
),

[_RAISE] = LAYOUT_all(
		_______
),

[_SPECIAL] = LAYOUT_all(
		_______
)
};


