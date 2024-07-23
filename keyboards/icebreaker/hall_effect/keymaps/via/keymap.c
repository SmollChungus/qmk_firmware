/* Copyright 2024 Matthijs Muller
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


#ifdef matrix_shenanigans // why the fuck does matrix.h include when i do custom keycodes?
#include "he_switch_matrix.h"
enum custom_keycodes {
    ACT_MODE_0 = SAFE_RANGE,  // Ensure these are added after the existing QMK keycodes
    ACT_MODE_1
};
#endif

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {


    [0] = LAYOUT(
        KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,   KC_5,     KC_6,     KC_7,      KC_8,     KC_9,     KC_0,     KC_MINS,   KC_EQL, KC_BSLS,  KC_DEL,  KC_GRV,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,   KC_T,     KC_Y,     KC_U,      KC_I,     KC_O,     KC_P,     KC_LBRC,   KC_RBRC,   KC_BSPC,  KC_HOME,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,   KC_G,     KC_H,     KC_J,      KC_K,     KC_L,     KC_SCLN,  KC_QUOT,   KC_ENTER,            KC_END,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,   KC_B,     KC_N,      KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,              KC_UP,    MO(1),
        KC_LCTL,  KC_LGUI,  KC_LALT,  KC_SPC,                                          KC_RALT,   KC_RCTL,                              KC_LEFT,   KC_DOWN,  KC_RGHT
    ),
#ifndef matrix_shenanigans
    [1] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,   KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,    KC_F12, _______,  _______,  QK_BOOT,
        _______,  _______,  KC_UP,    _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  KC_PGUP,
        _______,  KC_LEFT,  KC_DOWN,  KC_RIGHT, _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,            KC_PGDN,
        _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_MPLY,             KC_VOLU,  _______,
        _______,  _______,  _______,  _______,                                         _______,  _______,                           KC_MPRV,  KC_VOLD,  KC_MNXT
    )
#endif
#ifdef matrix_shenanigans
    [1] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,   KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,    KC_F12,   _______,  QK_BOOT,
        _______,  _______,  KC_UP,    _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  KC_PGUP,
        _______,  KC_LEFT,  KC_DOWN,  KC_RIGHT, _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,            KC_PGDN,
        _______,  _______,  _______,  _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_MPLY,             KC_VOLU,  _______,
        ACT_MODE_0,  ACT_MODE_1,  _______,  _______,                                         _______,                                  KC_MPRV,  KC_VOLD,  KC_MNXT
    )
#endif
};

#ifdef matrix_shenanigans
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case ACT_MODE_0:
            if (record->event.pressed) {
                he_config.he_actuation_mode = 0;
                uprintf("Actuation Mode set to 0\n");
            }
            return false;

        case ACT_MODE_1:
            if (record->event.pressed) {
                he_config.he_actuation_mode = 1;
                uprintf("Actuation Mode set to 1\n");
            }
            return false;

        default:
            return true;
    }
}
#endif
