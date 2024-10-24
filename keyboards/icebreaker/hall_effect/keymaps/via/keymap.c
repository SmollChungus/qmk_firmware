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
//keymap.c
#include QMK_KEYBOARD_H
#include "he_switch_matrix.h"

extern uint8_t console_output;

enum custom_keycodes {
    VERB0 = SAFE_RANGE,
    VERB1,
    VERB2,
    VERB3,
    VERB4,
    VERB5,
    APCM,
    RTM,
    KCM_ON,
    KCM_OFF,
    KCM_TOG,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {


    [0] = LAYOUT(
        KC_ESC,   KC_1,     KC_2,     KC_3,     KC_4,   KC_5,     KC_6,     KC_7,      KC_8,     KC_9,     KC_0,     KC_MINS,   KC_EQL, KC_BSLS,  KC_DEL,  KC_GRV,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,   KC_T,     KC_Y,     KC_U,      KC_I,     KC_O,     KC_P,     KC_LBRC,   KC_RBRC,   KC_BSPC,  KC_HOME,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,   KC_G,     KC_H,     KC_J,      KC_K,     KC_L,     KC_SCLN,  KC_QUOT,   KC_ENTER,            KC_END,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,     KC_V,   KC_B,     KC_N,      KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,              KC_UP,    MO(1),
        KC_LCTL,  KC_LGUI,  KC_LALT,  KC_SPC,                                          KC_RALT,   KC_RCTL,                              KC_LEFT,   KC_DOWN,  KC_RGHT
    ),
    [1] = LAYOUT(
        KC_ESC,   KC_F1,    KC_F2,    KC_F3,    KC_F4,   KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,    KC_F12, _______,  _______,  QK_BOOT,
        _______,  APCM,  RTM, _______, _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  KC_PGUP,
        _______,  KCM_ON,  KCM_OFF,  KCM_TOG, _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,            KC_PGDN,
        _______,  VERB1, VERB5,  VERB0,  _______, _______,  _______,  _______,  _______,  _______,   _______,  KC_MPLY,             RGB_HUI,  _______,
        _______,  _______,  _______,  _______,                                         _______,  _______,                           RGB_TOG,  RGB_HUD,  KC_MNXT
    )

};

// Blinking variables
static bool is_blinking = false;
static uint8_t blink_count = 0;
static uint16_t blink_timer = 0;
static uint8_t blink_hue = 0;
static uint8_t blink_leds[] = {28, 32, 33, 34}; // LEDs to blink (adjust indices as needed)

static uint8_t saved_mode;
static HSV saved_hsv;

void start_mode_blink(uint8_t hue) {
    // Save current RGB settings using API functions
    saved_mode = rgblight_get_mode();
    saved_hsv = rgblight_get_hsv();

    // Start the blinking process
    is_blinking = true;
    blink_count = 6; // Total on/off cycles
    blink_timer = timer_read();
    blink_hue = hue;
}



bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case VERB0:
            if (record->event.pressed) {
                console_output = 0;
                uprintf("[SYSTEM]: Logging Mode set to 0\n");
            }
            return false;

        case VERB1:
            if (record->event.pressed) {
                console_output = 1;
                uprintf("[SYSTEM]: Logging Mode set to 1\n");
            }
            return false;

        case VERB2:
            if (record->event.pressed) {
                console_output = 2;
                uprintf("[SYSTEM]: Logging Mode set to 2 (blocking keystrokes)\n");
            }
            return false;

        case VERB3:
            if (record->event.pressed) {
                console_output = 3;
                uprintf("[SYSTEM]: Logging Mode set to 3(none)\n");
            }
            return false;

        case VERB4:
            if (record->event.pressed) {
                console_output = 4;
                uprintf("[SYSTEM]: Logging Mode set to 4\n");
            }
            return false;

        case VERB5:
            if (record->event.pressed) {
                console_output = 5;
                uprintf("[SYSTEM]: Logging Mode set to 5\n");
            }
            return false;

        case APCM:
            if (record->event.pressed) {
                uprintf("[SYSTEM]: Actuation Point Control Mode set\n");
                uprintf("[PCB_SETTINGS]: APC MODE\n");
                he_config.he_actuation_mode = 0;
                eeprom_he_config.he_actuation_mode = 0;
                
                start_mode_blink(85); // green
            }
            return false;

        case RTM:
            if (record->event.pressed) {
                uprintf("[SYSTEM]: Rapid Trigger Mode set\n");
                uprintf("[PCB_SETTINGS]: RT MODE\n");
                he_config.he_actuation_mode = 1;
                eeprom_he_config.he_actuation_mode = 1;

                // Start blinking in red hue for RT mode
                start_mode_blink(0); // 0 is red in HSV
            }
            return false;

        case KCM_ON:
            if (record->event.pressed) {
                uprintf("[SYSTEM]: Key Cancellation Mode on\n");
                he_config.he_keycancel = true;
                eeprom_he_config.he_keycancel = true;
            }
            return false;
        case KCM_OFF:
            if (record->event.pressed) {
                uprintf("[SYSTEM]: Key Cancellation Mode off\n");
                he_config.he_keycancel = false;
                eeprom_he_config.he_keycancel = false;
            }
            return false;

        case KCM_TOG:
            if (record->event.pressed) {
                uprintf("[SYSTEM]: Key Cancellation Mode: %d\n", he_config.he_keycancel);
                he_config.he_keycancel = !he_config.he_keycancel;
                eeprom_he_config.he_keycancel = he_config.he_keycancel;
            }
            return false;

        default:
            return true;
    }
}

void matrix_scan_user(void) {
    if (is_blinking) {
        if (timer_elapsed(blink_timer) > 200) { // Adjust timing as needed
            // Toggle LEDs
            bool leds_on = (blink_count % 2 == 0);
            for (uint8_t i = 0; i < sizeof(blink_leds); i++) {
                uint8_t led_index = blink_leds[i];
                if (leds_on) {
                    // Turn on LED with specified hue
                    rgblight_sethsv_at(blink_hue, 255, 255, led_index);
                } else {
                    // Turn off LED
                    rgblight_sethsv_at(0, 0, 0, led_index);
                }
            }
            rgblight_set();
            blink_count--;
            blink_timer = timer_read();

            if (blink_count == 0) {
                // Blinking finished
                is_blinking = false;
                // Restore previous RGB settings
                rgblight_mode_noeeprom(saved_mode);
                rgblight_sethsv_noeeprom(saved_hsv.h, saved_hsv.s, saved_hsv.v);
                rgblight_set();
            }

        }
    }
}

