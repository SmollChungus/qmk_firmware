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
#include QMK_KEYBOARD_H

enum layer_names {
  _BASE,
  _LOWER,
  _RAISE,
  _ADJUST
};

#define LOWER  MO(_LOWER)
#define RAISE  MO(_RAISE)



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT_all(
    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,          KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,          KC_H,    KC_J,    KC_K,    KC_L,    KC_ENT,
    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,          KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                               KC_BSPC, LOWER,         KC_SPC,  RAISE
  ),

  [_LOWER] = LAYOUT_all(
    KC_1,   KC_2,   KC_3,   KC_4,   KC_5,              KC_6,  KC_7,  KC_8,  KC_9,  KC_0,
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,         KC_F6, KC_F7, KC_F8, KC_F9, KC_F10,
    KC_F11,  KC_F12,  KC_CAPS, XXXXXXX, XXXXXXX,       RESET,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                              _______, _______,        _______, _______
  ),

  [_RAISE] = LAYOUT_all(
    KC_TAB,   KC_F2,   KC_F3,   KC_F4,   KC_F5,         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_PSCR,
    KC_LSFT,   KC_F7,   KC_F8,   KC_F9,   KC_F10,        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    KC_LCTL,  KC_F12,  KC_CAPS, XXXXXXX, XXXXXXX,       RESET,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                              _______, _______,        _______, _______
  ),

  [_ADJUST] = LAYOUT_all(
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,         XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_PSCR,
    KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
    KC_F11,  KC_F12,  KC_CAPS, XXXXXXX, XXXXXXX,       RESET,   XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                              _______, _______,        _______, _______
  ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}
