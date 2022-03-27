/*
Copyright 2022 smollchunguss

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

#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	[0] = LAYOUT_midi70(
                 KC_F1,     KC_F2,   KC_F3,  KC_F4,  KC_F5,  KC_F6,  KC_F7,  KC_F8,   KC_F9,   KC_F10 , KC_F11,  KC_F12,  KC_F13,
	    KC_1,    KC_2,      KC_3,    KC_4,   KC_5,   KC_6,   KC_7,   KC_8,   KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_GRV,
	    KC_TAB,  KC_Q,      KC_W,    KC_E,   KC_R,   KC_T,   KC_Y,   KC_U,   KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
	    KC_CAPS, MI_VEL_10, MI_CH1,  MI_CH2, KC_F,   KC_G,   KC_H,   KC_J,   KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
	    KC_LSFT,            KC_Z,    KC_X,   KC_C,   KC_V,   KC_B,   KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        MI_ON,   MI_OFF,   MI_TOG,                           KC_SPC,                                    TG(1),   KC_RGUI, KC_RCTL
	),

/*	            _________________________________________
              /                                           \
	         /					                           \
	        /                                                \
           /                                                  \
	      /                                                    \
         /                                                      |
        /		         _______________                        |
       /                |	    |       |                      /|
      /                 |=======+=======|                     / |
     /                  |_______|_______|                    /  |
    /_______________________________________________________/   |
    |                                                       |   /
    |   _   _       _   _   _       _   _       _   _   _   |  /
    |__//|_//|_____//|_//|_//|_____//|_//|_____//|_//|_//|__| /
   /  /// ///  /  /// /// ///  /  /// ///  /  /// /// ///  / /
  /  ||/ ||/  /  ||/ ||/ ||/  /  ||/ ||/  /  ||/ ||/ ||/  / /
 /___/___/___/___/___/___/___/___/___/___/___/___/___/___/ /
 |___|___|___|___|___|___|___|___|___|___|___|___|___|___|/
        \   /                                 \   /
         | ||                                  | ||
         | ||                                  | ||
         |_|/                                  |_|/
*/

	[1] = LAYOUT_midi70(
                  MI_Cs_1, MI_Ds_1, KC_TRNS, MI_Fs_1, MI_Gs_1,  MI_As_1, KC_TRNS, MI_Cs_2,  MI_Ds_2,   KC_TRNS,   MI_Fs_2,   MI_Gs_2,  MI_As_2,
	    MI_C_1,   MI_D_1,  MI_E_1,  MI_F_1,  MI_G_1,  MI_A_1,   MI_B_1,  MI_C_2,  MI_D_2,   MI_E_2,    MI_F_2,    MI_G_2,    MI_A_2,   MI_B_2,
	    MI_TRNSU, MI_VELU, MI_VELD, MI_SUS,  MI_SOST,    KC_T,     KC_Y,    KC_U,    KC_I,     KC_O,      KC_P,      KC_LBRC,   KC_RBRC,  KC_BSPC,
	    MI_TRNSD, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,     KC_H,    KC_J,    KC_K,     KC_L,      KC_SCLN,   KC_QUOT,   KC_ENT,
	    KC_LSFT,           KC_Z,    KC_X,    KC_C,    KC_V,     KC_B,    KC_N,    KC_M,     KC_COMM,   KC_DOT,    KC_SLSH,   KC_RSFT,
        KC_LOCK,  KC_LGUI, KC_LALT,                             KC_SPC,                                           TG(1),   KC_RGUI,   KC_RCTL
	),

	[2] = LAYOUT_midi70(
                 KC_F1,   KC_F2,  KC_F3,  KC_F4,  KC_F5,  KC_F6,  KC_F7,  KC_F8,   KC_F9,   KC_F10 , KC_F11,  KC_F12,  KC_F13,
	    KC_1,    KC_2,    KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   KC_8,   KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_GRV,
	    KC_TAB,  KC_Q,    KC_W,   KC_E,   KC_R,   KC_T,   KC_Y,   KC_U,   KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
	    KC_CAPS, KC_A,    KC_S,   KC_D,   KC_F,   KC_G,   KC_H,   KC_J,   KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
	    KC_LSFT,          KC_Z,   KC_X,   KC_C,   KC_V,   KC_B,   KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        KC_LCTL, KC_LGUI, KC_LALT,                        KC_SPC,                                    KC_RALT, KC_RGUI, KC_RCTL
	),

	[3] = LAYOUT_midi70(
                 KC_F1,   KC_F2,  KC_F3,  KC_F4,  KC_F5,  KC_F6,  KC_F7,  KC_F8,   KC_F9,   KC_F10 , KC_F11,  KC_F12,  KC_F13,
	    KC_1,    KC_2,    KC_3,   KC_4,   KC_5,   KC_6,   KC_7,   KC_8,   KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSLS, KC_GRV,
	    KC_TAB,  KC_Q,    KC_W,   KC_E,   KC_R,   KC_T,   KC_Y,   KC_U,   KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSPC,
	    KC_CAPS, KC_A,    KC_S,   KC_D,   KC_F,   KC_G,   KC_H,   KC_J,   KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,
	    KC_LSFT,          KC_Z,   KC_X,   KC_C,   KC_V,   KC_B,   KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        KC_LCTL, KC_LGUI, KC_LALT,                        KC_SPC,                                    KC_RALT, KC_RGUI, KC_RCTL
	)
};
