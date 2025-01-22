/* Copyright 2025 Matthijs Muller
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include "icebreaker_rgb.h"
#include "hall_effect.h"

/* Custom LED animations to visualize calibration, settings and mode switching */

static const uint8_t sensor_to_led_map[SENSOR_COUNT] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
    30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
    58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45,
    59, 60, 61, 62, 63, 64, 65, 66, 67
};

static const HSV purple = {191, 255, 255};
static const HSV red = {0, 255, 255};
static const HSV orange = {45, 255, 255};
static const HSV green = {90, 255, 255};


static uint8_t warning_leds[MAX_WARNING_LEDS];
static uint8_t warning_led_count = 0;

static calibration_led_t calibration_leds[SENSOR_COUNT];
static bool calibration_changes_pending = false;

uint8_t saved_calibration_mode;
HSV saved_calibration_hsv;

bool slider_visualization_active = false;
uint16_t slider_timeout_timer = 0;

int8_t last_moved_slider = -1; // -1 = no slider active
bool slider_active = false;

typedef struct {
    slider_type_t type;
    uint8_t min;
    uint8_t max;
} slider_range_t;

const slider_range_t slider_ranges[SLIDER_TYPE_MAX] = {
    [SLIDER_TYPE_ACTUATION] = {SLIDER_TYPE_ACTUATION, 10, 90},
    [SLIDER_TYPE_RELEASE] = {SLIDER_TYPE_RELEASE, 10, 90},
    [SLIDER_TYPE_RTP_DEADZONE] = {SLIDER_TYPE_RTP_DEADZONE, 15, 60},
    [SLIDER_TYPE_RTP_ENGAGE] = {SLIDER_TYPE_RTP_ENGAGE, 5, 20},
    [SLIDER_TYPE_RTP_DISENGAGE] = {SLIDER_TYPE_RTP_DISENGAGE, 5, 20},
};

slider_type_t current_slider_type = SLIDER_TYPE_MAX;
static uint8_t last_leds_lit = 0xFF;
static uint16_t last_update_time = 0;

uint8_t latest_slider_value = 0;
bool final_slider_update_pending = false;

/*RGB calibration visualization*/

void calibration_warning(void) {
    warning_led_count = 0;
    HSV original_color = rgblight_get_hsv();
    
    for (int i = 0; i < SENSOR_COUNT; i++) {
        uint16_t ceiling = eeprom_he_key_configs[i].noise_ceiling;
        if (ceiling < CEILING_GOOD) {
            warning_leds[warning_led_count] = i;
            warning_led_count++;
        }
    }

    if (warning_led_count > 0) {
        for (int flash = 0; flash < FLASH_COUNT; flash++) {
            for (int i = 0; i < warning_led_count; i++) {
                rgblight_sethsv_at(orange.h, orange.s, orange.v, 
                    sensor_to_led_map[warning_leds[i]]);
            }
            rgblight_set();
            wait_ms(FLASH_DURATION);

            for (int i = 0; i < warning_led_count; i++) {
                rgblight_sethsv_at(original_color.h, original_color.s, original_color.v, 
                    sensor_to_led_map[warning_leds[i]]);
            }
            rgblight_set();
            wait_ms(FLASH_DURATION);
        }
    }
}

void flash_rgb_warning(uint8_t sensor_id, uint8_t notification_type) {
    HSV flash_color;
    switch (notification_type) {
        case RGB_NOTIFICATION_CALIBRATION_WARNING:
            flash_color = orange;
            break;
        case RGB_NOTIFICATION_MODE_NORMAL:
            flash_color = green;
            break;
        case RGB_NOTIFICATION_MODE_RAPID_TRIGGER:
            flash_color = purple;
            break;
        case RGB_NOTIFICATION_MODE_KEY_CANCEL:
            flash_color = red;
            break;
        default:
            return;
    }

    HSV original_color = rgblight_get_hsv();
    
    for (int i = 0; i < FLASH_COUNT; i++) {
        rgblight_sethsv_at(flash_color.h, flash_color.s, flash_color.v, sensor_to_led_map[sensor_id]);
        wait_ms(FLASH_DURATION);
        rgblight_sethsv_at(original_color.h, original_color.s, original_color.v, sensor_to_led_map[sensor_id]);
        wait_ms(FLASH_DURATION);
    }
}

void start_calibration_rgb(void) {
    // Save current RGB settings
    saved_calibration_mode = rgblight_get_mode();
    saved_calibration_hsv = rgblight_get_hsv();

    for (int i = 0; i < SENSOR_COUNT; i++) {
        calibration_leds[i].sensor_id = i;
        calibration_leds[i].state = LED_STATE_UNCALIBRATED;
        rgblight_sethsv_at(red.h, red.s, red.v, sensor_to_led_map[i]);
    }
    rgblight_set();
    calibration_changes_pending = false; 
}

void update_calibration_rgb(uint8_t sensor_id, uint16_t ceiling) {
    uint8_t new_state;
    if(sensor_id == 68) return; //encoder
    
    if (ceiling >= CEILING_GOOD) {
        new_state = LED_STATE_CALIBRATED;
    } else if (ceiling >= CEILING_MEDIUM) {
        new_state = LED_STATE_PARTIAL;
    } else {
        new_state = LED_STATE_UNCALIBRATED;
    }

    if (calibration_leds[sensor_id].state != new_state) {
        calibration_leds[sensor_id].state = new_state;
        calibration_changes_pending = true;
    }
}

void apply_calibration_changes_rgb(void) {
    if (!calibration_changes_pending) return;

    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (i == 68) continue; // Skip encoder

        HSV color;
        switch (calibration_leds[i].state) {
            case LED_STATE_CALIBRATED:
                color = green;
                break;
            case LED_STATE_PARTIAL:
                color = orange;
                break;
            default:
                color = red;
                break;
        }
        rgblight_sethsv_at(color.h, color.s, color.v, sensor_to_led_map[i]);
    }
    rgblight_set();
    calibration_changes_pending = false;
}

/* Slider RGB visualization */

void end_calibration_visual(void) {
    rgblight_mode_noeeprom(saved_calibration_mode);
    rgblight_sethsv_noeeprom(saved_calibration_hsv.h, saved_calibration_hsv.s, saved_calibration_hsv.v);
    rgblight_set();
}

void handle_final_led_update(void) {
    if (final_slider_update_pending && slider_visualization_active) {
        update_slider_visualization(latest_slider_value);
        final_slider_update_pending = false;
    }
}

void start_slider_visualization(uint8_t value) {
    if (!slider_visualization_active) {
        saved_calibration_mode = rgblight_get_mode();
        saved_calibration_hsv = rgblight_get_hsv();
        slider_visualization_active = true;
    }
    slider_timeout_timer = timer_read();
    update_slider_visualization(value);
}

void update_slider_visualization(uint8_t value) {
    if (!slider_visualization_active) {
        return;
    }
    latest_slider_value = value;

    uint16_t elapsed = timer_elapsed(last_update_time);
    if (elapsed < SLIDER_UPDATE_INTERVAL) {
        final_slider_update_pending = true;
        return;
    }

    final_slider_update_pending = false;

    uint8_t min = slider_ranges[current_slider_type].min;
    uint8_t max = slider_ranges[current_slider_type].max;

    if (value < min) value = min;
    if (value > max) value = max;

    uint8_t leds_to_light = 1 + ((value - min) * (TOP_ROW_LED_COUNT - 1)) / (max - min);

    if (leds_to_light < 1) {
        leds_to_light = 1;
    } else if (leds_to_light > TOP_ROW_LED_COUNT) {
        leds_to_light = TOP_ROW_LED_COUNT;
    }

    HSV current_color;
    switch (current_slider_type) {
        case SLIDER_TYPE_ACTUATION:
            current_color = green;
            break;
        case SLIDER_TYPE_RELEASE:
            current_color = red;
            break;
        case SLIDER_TYPE_RTP_DEADZONE:
            current_color = orange;
            break;
        case SLIDER_TYPE_RTP_ENGAGE:
            current_color = purple;
            break;
        case SLIDER_TYPE_RTP_DISENGAGE:
            current_color = purple;
            break;
        default:
            current_color = red;
            break;
    }

    if (leds_to_light != last_leds_lit) {
        for (uint8_t i = 0; i < TOP_ROW_LED_COUNT; i++) {
            if (i < leds_to_light) {
                rgblight_sethsv_at(current_color.h, current_color.s, current_color.v, i); 
            } else {
                rgblight_sethsv_at(0, 0, 0, i);
            }
        }
        rgblight_set();
        last_leds_lit = leds_to_light;
    }

    last_update_time = timer_read();
    slider_timeout_timer = last_update_time;
}

void end_slider_visualization(void) {
    rgblight_mode_noeeprom(saved_calibration_mode);
    rgblight_sethsv_noeeprom(saved_calibration_hsv.h, saved_calibration_hsv.s, saved_calibration_hsv.v);
    rgblight_set();
    slider_visualization_active = false;
}
