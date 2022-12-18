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
		UC(0x40B),    KC_ESC,   KC_1, KC_2,  KC_3,     KC_4,  KC_5,  KC_6,
		UC(0x7FFE),  KC_TAB,   KC_Q,        KC_W,     KC_E,  KC_R,  KC_T,
		UC_WIN,  LOWER,    KC_A,        KC_S,     KC_D,  KC_F,  KC_G,
		          KC_LSFT,  KC_Z,        KC_X,     KC_C,  KC_V,  KC_B,
		          KC_LCTL,               KC_LALT,  RAISE,        SPECIAL
),

[_LOWER] = LAYOUT_all(
		_______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______,          _______, _______, _______, _______,
		_______, _______, _______,          _______, _______, _______, _______,
		         _______, _______,          _______, _______, _______, _______,
		         _______,                   _______, _______,          _______
),

[_RAISE] = LAYOUT_all(
        _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______,          _______, _______, _______, _______,
		_______, _______, _______,          _______, _______, _______, _______,
		         _______, _______,          _______, _______, _______, _______,
		         _______,                   _______, _______,          _______
),

[_SPECIAL] = LAYOUT_all(
        _______, _______, _______, _______, _______, _______, _______, _______,
		_______, _______, _______,          _______, _______, _______, _______,
		_______, _______, _______,          _______, _______, _______, _______,
		         _______, _______,          _______, _______, _______, _______,
		         _______,                   _______, _______,          _______
)
};


#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_270; }

static void render_logo(void) {
    static const char PROGMEM sc_logo[] = {
       // 'qweqwe', 64x32px
// 'qweqwe', 32x32px
// 'qweqwe', 32x32px
// 'qweqwe', 32x32px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x70, 0xb8, 0xdc, 0xec,
0xf0, 0x78, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1c, 0xbf, 0xf9, 0xf9, 0xf7, 0xf9, 0xdb, 0x0b, 0x0e,
0x04, 0x0c, 0x0e, 0x1e, 0xde, 0xde, 0x6c, 0xa0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x23, 0x23, 0x3f, 0x1e, 0x0e, 0x16, 0x1c,
0x20, 0x3c, 0x34, 0x2e, 0x2e, 0x2e, 0x2f, 0x17, 0x3b, 0x1c, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    // Host Keyboard Layer Status
    oled_write_raw_P(sc_logo, sizeof(sc_logo));
}


	bool oled_task_user(void) {
		render_logo();
		oled_set_cursor(0,5);

		oled_write_ln_P(PSTR(""), false);

    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_ln_P(PSTR("Base"), false);
            break;
        case _LOWER:
            oled_write_ln_P(PSTR("Down"), false);
            break;
        case _RAISE:
            oled_write_ln_P(PSTR("Up"), false);
            break;
        case _SPECIAL:
            oled_write_ln_P(PSTR("Set"), false);
            break;
        default:
            oled_write_ln_P(PSTR("Undef"), false);
    }
	oled_write_ln_P(PSTR(""), false);
    // Host Keyboard LED Status
    led_t led_state = host_keyboard_led_state();
    oled_write_ln_P(led_state.num_lock ? PSTR("NUM ") : PSTR("    "), false);
    oled_write_ln_P(led_state.caps_lock ? PSTR("CAP ") : PSTR("    "), false);
    oled_write_ln_P(led_state.scroll_lock ? PSTR("SCR ") : PSTR("    "), false);

    render_logo();

    return false;
	}
#endif


