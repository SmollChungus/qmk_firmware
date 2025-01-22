/* Copyright 2025 Matthijs Muller
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
#include "hall_effect.h"
#include "icebreaker_rgb.h"


enum custom_keycodes {
    APCM = QK_KB_0,
    RTM,
    KCM,
    DEBUG0,
    DEBUG1,
    DEBUG2,
    DEBUG3,
    DEBUG4,
    DEBUG5,

};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {


    [0] = LAYOUT(
        KC_ESC,  KC_1,    KC_2,    KC_3, KC_4, KC_5,   KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,   KC_BSLS, KC_DEL,  KC_GRV,
        KC_TAB,  KC_Q,    KC_W,    KC_E, KC_R, KC_T,   KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC,  KC_BSPC, KC_HOME,
        KC_GRV,  KC_A,    KC_S,    KC_D, KC_F, KC_G,   KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENTER, KC_END,
        KC_LSFT, KC_Z,    KC_X,    KC_C, KC_V, KC_B,   KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, KC_UP,    MO(1),
        KC_LCTL, KC_LGUI, KC_LALT,             KC_SPC,                   KC_RALT, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT,  KC_MPLY
    ),
    [1] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,    KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  _______, _______, RGB_TOG,
        _______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, RGB_VAI,
        _______, _______, _______, _______,  _______, _______, _______, _______, _______, _______, _______, _______, KC_MPLY, RGB_VAD,
        MO(2),   _______, _______, _______,  _______, _______, _______, RGB_HUD, RGB_HUI, RGB_SAD, RGB_SAI, KC_MUTE, KC_VOLU, _______,
        _______, _______, _______,                    _______,                   _______, _______, KC_MPRV, KC_VOLD, KC_MNXT, _______
    ),
        [2] = LAYOUT(
        QK_BOOT, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, APCM,    RTM,     KCM,     _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, DEBUG0,  DEBUG1,  DEBUG2,  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______,                   _______,                   _______, _______, _______, _______, _______, _______
    )

};

extern uint8_t console_output;

// RGB variables
static bool is_blinking = false;
static uint8_t blink_count = 0;
static uint16_t blink_timer = 0;
static uint8_t blink_hue = 0;
static uint8_t blink_leds[] = {28, 32, 33, 34}; // WASD
static uint8_t saved_mode;
static HSV saved_hsv;

static uint16_t last_eeprom_write_timer = 0;

void start_mode_blink(uint8_t hue) {
    saved_mode = rgblight_get_mode();
    saved_hsv = rgblight_get_hsv();

    is_blinking = true;
    blink_count = 6; 
    blink_timer = timer_read();
    blink_hue = hue;
}

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] =   { ENCODER_CCW_CW(KC_VOLD, KC_VOLU)  },
    [1] =  { ENCODER_CCW_CW(RGB_VAD, RGB_VAI)  },
    [2] =  { ENCODER_CCW_CW(RGB_VAD, RGB_VAI)  },
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case APCM:
            if (record->event.pressed) {
                he_config.he_actuation_mode = 0;
                eeprom_he_config.he_actuation_mode = 0;
                start_mode_blink(85); //green
                eeconfig_update_kb_datablock(&eeprom_he_config);
            }
            return false;

        case RTM:
            if (record->event.pressed) {
                he_config.he_actuation_mode = 1;
                eeprom_he_config.he_actuation_mode = 1;
                start_mode_blink(0); // red
                eeconfig_update_kb_datablock(&eeprom_he_config);

            }
            return false;

        case KCM:
            if (record->event.pressed) {
                he_config.he_actuation_mode = 2;
                eeprom_he_config.he_actuation_mode = 2;
                start_mode_blink(170); // purple
                eeconfig_update_kb_datablock(&eeprom_he_config);
            }
            return false;

        case DEBUG0:
            if (record->event.pressed) {
                console_output = 0;
            }
            return false;

        case DEBUG1:
            if (record->event.pressed) {
                console_output = 1;
            }
            return false;

        case DEBUG2:
            if (record->event.pressed) {
                console_output = 2;
            }
            return false;

        default:
            return true;
    }
}

void keyboard_post_init_user(void) {
    calibration_warning();
}

void matrix_scan_user(void) {
    // Handle blinking
    if (is_blinking) {
        if (timer_elapsed(blink_timer) > 200) {
            bool leds_on = (blink_count % 2 == 0);
            for (uint8_t i = 0; i < sizeof(blink_leds); i++) {
                uint8_t led_index = blink_leds[i];
                if (leds_on) {
                    rgblight_sethsv_at(blink_hue, 255, 255, led_index);
                } else {
                    rgblight_sethsv_at(0, 0, 0, led_index);
                }
            }
            rgblight_set();
            blink_count--;
            blink_timer = timer_read();

            if (blink_count == 0) {
                is_blinking = false;
                // Restore previous RGB settings
                rgblight_mode_noeeprom(saved_mode);
                rgblight_sethsv_noeeprom(saved_hsv.h, saved_hsv.s, saved_hsv.v);
                rgblight_set();
            }
        }
    }

    // Handle EEPROM save with debounce
    if (eeprom_save_pending && timer_elapsed(eeprom_save_timer) > EEPROM_SAVE_DELAY) {
        if (timer_elapsed(last_eeprom_write_timer) > EEPROM_SAVE_DELAY) {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                eeprom_he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_actuation_threshold;
                eeprom_he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_release_threshold;
                he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_actuation_threshold;
                he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_release_threshold;
            }
            eeconfig_update_user_datablock(&eeprom_he_key_configs);
            eeprom_save_pending = false;
            last_eeprom_write_timer = timer_read();
        } else {
            /* Do nothing */
            }
        }

    if (slider_active && timer_elapsed(slider_timeout_timer) > SLIDER_TIMEOUT) {
        end_slider_visualization();
        slider_active = false;
        last_moved_slider = -1;
    }
    if (slider_active && final_slider_update_pending && timer_elapsed(slider_timeout_timer) > 500) {
        handle_final_led_update();
    }
}

// todo delete
/* bool encoder_update_user(uint8_t index, bool clockwise) {
    print("Encoder event: ");
    if (clockwise) {
        print("CW\n");
    } else {
        print("CCW\n");
    }
    return true;
} */