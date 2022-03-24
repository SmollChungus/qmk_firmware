/* Copyright 2021 Smollchungus
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
    KC_A,    KC_B,      KC_U, KC_I,
    KC_D,   KC_C,    KC_H,    KC_J,
    KC_E,   KC_F,   KC_G,    KC_K,
              KC_M,    KC_L
),

[_LOWER] = LAYOUT_all(
    KC_A,    KC_A,      KC_A, KC_A,
    KC_F15,   KC_F16,    KC_BTN3,    KC_I,
    KC_LCTL,   KC_BTN1,   KC_F23,    KC_C,
              KC_F11,    KC_F12
),

[_RAISE] = LAYOUT_all(
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
             KC_TRNS, KC_TRNS
),

[_SPECIAL] = LAYOUT_all(
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
             KC_TRNS, KC_TRNS
)
};
