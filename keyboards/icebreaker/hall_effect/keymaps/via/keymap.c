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
#include "custom_rgb.h"


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
    KCM,
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
        _______,  APCM,  RTM, KCM, _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,  _______,  KC_PGUP,
        _______,  _______,  _______,  _______, _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,   _______,            KC_PGDN,
        _______,  VERB1, VERB5,  VERB0,  VERB2, _______,  _______,  _______,  _______,  _______,   _______,  KC_MPLY,             RGB_HUI,  _______,
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


static uint16_t last_eeprom_write_timer = 0;

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
                eeconfig_update_kb_datablock(&eeprom_he_config);

            }
            return false;

        case RTM:
            if (record->event.pressed) {
                uprintf("[SYSTEM]: Rapid Trigger Mode set\n");
                uprintf("[PCB_SETTINGS]: RT MODE\n");
                he_config.he_actuation_mode = 1;
                eeprom_he_config.he_actuation_mode = 1;
                start_mode_blink(0); // red
                eeconfig_update_kb_datablock(&eeprom_he_config);

            }
            return false;

        case KCM:
            if (record->event.pressed) {
                uprintf("[SYSTEM]: Key Cancel Mode set\n");
                he_config.he_actuation_mode = 2;
                eeprom_he_config.he_actuation_mode = 2;
                start_mode_blink(170); // purple
                eeconfig_update_kb_datablock(&eeprom_he_config);
            }
            return false;

        default:
            return true;
    }
}

void matrix_scan_user(void) {

    // Handle blinking
    if (is_blinking) {
        if (timer_elapsed(blink_timer) > 200) { // Adjust timing as needed
            uprintf("[DEBUG]: Handling blinking. Blink count: %d\n", blink_count);
            bool leds_on = (blink_count % 2 == 0);
            for (uint8_t i = 0; i < sizeof(blink_leds); i++) {
                uint8_t led_index = blink_leds[i];
                if (leds_on) {
                    // Turn on LED with specified hue
                    rgblight_sethsv_at(blink_hue, 255, 255, led_index);
                    uprintf("[DEBUG]: LED %d turned ON with hue %d\n", led_index, blink_hue);
                } else {
                    // Turn off LED
                    rgblight_sethsv_at(0, 0, 0, led_index);
                    uprintf("[DEBUG]: LED %d turned OFF\n", led_index);
                }
            }
            rgblight_set();
            blink_count--;
            blink_timer = timer_read();
            uprintf("[DEBUG]: Decremented blink_count to %d\n", blink_count);

            if (blink_count == 0) {
                // Blinking finished
                is_blinking = false;
                uprintf("[DEBUG]: Blinking finished. Restoring previous RGB settings.\n");
                // Restore previous RGB settings
                rgblight_mode_noeeprom(saved_mode);
                rgblight_sethsv_noeeprom(saved_hsv.h, saved_hsv.s, saved_hsv.v);
                rgblight_set();
            }
        }
    }

    // Handle EEPROM save with debounce
    if (eeprom_save_pending && timer_elapsed(eeprom_save_timer) > EEPROM_SAVE_DELAY) {
        uprintf("[DEBUG]: EEPROM save pending detected.\n");
        // Check if minimum interval has passed since last EEPROM write
        if (timer_elapsed(last_eeprom_write_timer) > EEPROM_SAVE_DELAY) {
            uprintf("[DEBUG]: Minimum EEPROM write interval passed. Proceeding with save.\n");
            for (int i = 0; i < SENSOR_COUNT; i++) {
                eeprom_he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_actuation_threshold;
                eeprom_he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_release_threshold;
                he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_actuation_threshold;
                he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_release_threshold;
                uprintf("[DEBUG]: Sensor %d thresholds set. Actuation: %d, Release: %d\n",
                        i,
                        eeprom_he_key_configs[i].he_actuation_threshold,
                        eeprom_he_key_configs[i].he_release_threshold);
            }
            eeconfig_update_user_datablock(&eeprom_he_key_configs);
            eeprom_save_pending = false;
            uprintf("[SYSTEM]: Settings auto-saved to EEPROM\n");
            last_eeprom_write_timer = timer_read(); // Reset the debounce timer
            uprintf("[DEBUG]: Updated last_eeprom_write_timer to %u\n", last_eeprom_write_timer);
        } else {
            uprintf("[DEBUG]: EEPROM save deferred. Minimum interval not yet passed.\n");
        }
    }

    // Handle slider animation timeout
    if (slider_active && timer_elapsed(slider_timeout_timer) > SLIDER_TIMEOUT) {
        uprintf("[DEBUG]: Slider animation timeout reached. Ending visualization.\n");
        end_slider_visualization();
        slider_active = false;
        last_moved_slider = -1;
    }
    if (slider_active && final_slider_update_pending && timer_elapsed(slider_timeout_timer) > 500) {
        uprintf("[DEBUG]: final update slider RGB\n");
        handle_final_led_update();
    }
}