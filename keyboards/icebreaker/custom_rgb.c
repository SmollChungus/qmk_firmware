#include "custom_rgb.h"
#include "he_switch_matrix.h"

static const uint8_t sensor_to_led_map[SENSOR_COUNT] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,  // Row 0
    30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16,      // Row 1
    31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,          // Row 2
    58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45,          // Row 3
    59, 60, 61, 62, 63, 64, 65, 66, 67                               // Row 4
};

static const HSV warning_color = {28, 255, 255};     // Orange
static const HSV normal_color = {85, 255, 255};      // Green
static const HSV rapid_color = {191, 255, 255};      // Purple
static const HSV cancel_color = {0, 255, 255};       // Red

static const HSV uncalibrated_color = {0, 255, 255};     // Red
static const HSV partial_color = {28, 255, 255};         // Orange
static const HSV calibrated_color = {85, 255, 255};      // Green

typedef struct {
    uint8_t sensor_id;
} warning_led_t;

static warning_led_t warning_leds[MAX_WARNING_LEDS];
static uint8_t warning_led_count = 0;

static calibration_led_t calibration_leds[SENSOR_COUNT];
static bool calibration_changes_pending = false;

uint8_t saved_calibration_mode;
HSV saved_calibration_hsv;


// via slider viz
bool slider_visualization_active = false;
uint16_t slider_timeout_timer = 0;

int8_t last_moved_slider = -1; // -1 means no slider active
bool slider_active = false;

//sliderdebug
typedef struct {
    slider_type_t type;
    uint8_t min;
    uint8_t max;
} slider_range_t;

// Array holding range information for each slider type
const slider_range_t slider_ranges[SLIDER_TYPE_MAX] = {
    [SLIDER_TYPE_ACTUATION] = {SLIDER_TYPE_ACTUATION, 10, 90},
    [SLIDER_TYPE_RELEASE] = {SLIDER_TYPE_RELEASE, 10, 90},
    [SLIDER_TYPE_RTP_DEADZONE] = {SLIDER_TYPE_RTP_DEADZONE, 15, 60},
};

slider_type_t current_slider_type = SLIDER_TYPE_MAX;
static uint8_t last_leds_lit = 0xFF; // Initialize to an invalid value
static uint16_t last_update_time = 0;

void calibration_warning(void) {
    warning_led_count = 0;
    HSV original_color = rgblight_get_hsv();
    
    // First collect all warnings
    for (int i = 0; i < SENSOR_COUNT; i++) {
        uint16_t ceiling = eeprom_he_key_configs[i].noise_ceiling;
        if (ceiling < 650) {
            warning_leds[warning_led_count].sensor_id = i;
            warning_led_count++;
            uprintf("Warning: Sensor %d has low ceiling value: %d\n", i, ceiling);
        }
    }

    // If we have warnings, flash all affected LEDs together
    if (warning_led_count > 0) {
        for (int flash = 0; flash < FLASH_COUNT; flash++) {
            // Turn on all warning LEDs
            for (int i = 0; i < warning_led_count; i++) {
                rgblight_sethsv_at(warning_color.h, warning_color.s, warning_color.v, 
                    sensor_to_led_map[warning_leds[i].sensor_id]);
            }
            rgblight_set();
            wait_ms(FLASH_DURATION);

            // Turn off all warning LEDs
            for (int i = 0; i < warning_led_count; i++) {
                rgblight_sethsv_at(original_color.h, original_color.s, original_color.v, 
                    sensor_to_led_map[warning_leds[i].sensor_id]);
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
            flash_color = warning_color;
            break;
        case RGB_NOTIFICATION_MODE_NORMAL:
            flash_color = normal_color;
            break;
        case RGB_NOTIFICATION_MODE_RAPID_TRIGGER:
            flash_color = rapid_color;
            break;
        case RGB_NOTIFICATION_MODE_KEY_CANCEL:
            flash_color = cancel_color;
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

    // Initialize all LEDs to uncalibrated state (red)
    for (int i = 0; i < SENSOR_COUNT; i++) {
        calibration_leds[i].sensor_id = i;
        calibration_leds[i].state = LED_STATE_UNCALIBRATED;
        rgblight_sethsv_at(uncalibrated_color.h, uncalibrated_color.s, uncalibrated_color.v, sensor_to_led_map[i]);
    }
    rgblight_set();
    calibration_changes_pending = false;  // Since we've already applied the changes
}

void update_calibration_rgb(uint8_t sensor_id, uint16_t ceiling) {
    uint8_t new_state;
    
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
        HSV color;
        switch (calibration_leds[i].state) {
            case LED_STATE_CALIBRATED:
                color = calibrated_color;
                break;
            case LED_STATE_PARTIAL:
                color = partial_color;
                break;
            default:
                color = uncalibrated_color;
                break;
        }
        rgblight_sethsv_at(color.h, color.s, color.v, sensor_to_led_map[i]);
    }
    rgblight_set();
    calibration_changes_pending = false;
}

void end_calibration_visual(void) {
    // Restore previous RGB settings
    rgblight_mode_noeeprom(saved_calibration_mode);
    rgblight_sethsv_noeeprom(saved_calibration_hsv.h, saved_calibration_hsv.s, saved_calibration_hsv.v);
    rgblight_set();
}


void start_slider_visualization(uint8_t value) {
    uprintf("[DEBUG]: Starting slider visualization with value: %d\n", value);
    if (!slider_visualization_active) {
        saved_calibration_mode = rgblight_get_mode();
        saved_calibration_hsv = rgblight_get_hsv();
        slider_visualization_active = true;
        uprintf("[DEBUG]: Slider visualization activated. Saved RGB mode: %d, HSV: (%d, %d, %d)\n",
                saved_calibration_mode, saved_calibration_hsv.h, saved_calibration_hsv.s, saved_calibration_hsv.v);
    }
    slider_timeout_timer = timer_read();
    uprintf("[DEBUG]: slider_timeout_timer set to %u\n", slider_timeout_timer);
    update_slider_visualization(value);
}

// Add a debounce interval (in milliseconds)

void update_slider_visualization(uint8_t value) {
    if (!slider_visualization_active) {
        uprintf("[DEBUG]: Slider visualization is not active. Exiting update_slider_visualization.\n");
        return;
    }

    // Use timer_elapsed to correctly calculate elapsed time
    uint16_t elapsed = timer_elapsed(last_update_time);
    if (elapsed < SLIDER_UPDATE_INTERVAL) {
        uprintf("[DEBUG]: Slider visualization update skipped. Time since last update: %u ms\n",
                elapsed);
        return; // Skip update to enforce rate limiting
    }

    uprintf("[DEBUG]: Updating slider visualization with value: %d\n", value);

    // Validate current_slider_type
    if (current_slider_type >= SLIDER_TYPE_MAX) {
        uprintf("[ERROR]: Invalid slider type. Cannot update visualization.\n");
        return;
    }

    // Retrieve min and max for the current slider
    uint8_t min = slider_ranges[current_slider_type].min;
    uint8_t max = slider_ranges[current_slider_type].max;

    // Ensure value is within min and max
    if (value < min) value = min;
    if (value > max) value = max;

    // Calculate the proportion of LEDs to light based on slider range
    uint8_t leds_to_light = ((value - min) * TOP_ROW_LED_COUNT) / (max - min);

    uprintf("[DEBUG]: Calculated leds_to_light: %d (value: %d, min: %d, max: %d)\n",
            leds_to_light, value, min, max);

    // Only update if the number of LEDs to light has changed
    if (leds_to_light != last_leds_lit) {
        uprintf("[DEBUG]: leds_to_light changed from %d to %d. Updating LEDs.\n",
                last_leds_lit, leds_to_light);
        for (uint8_t i = 0; i < TOP_ROW_LED_COUNT; i++) {
            if (i <= leds_to_light) {
                rgblight_sethsv_at(0, 0, 255, i);  // Purple for lit LEDs
                uprintf("[DEBUG]: LED %d set to Purple.\n", i);
            } else {
                rgblight_sethsv_at(0, 0, 0, i);    // Off for unlit LEDs
                uprintf("[DEBUG]: LED %d turned OFF.\n", i);
            }
        }
        rgblight_set();
        last_leds_lit = leds_to_light;
        uprintf("[DEBUG]: rgblight_set() called. Slider visualization updated.\n");
    }

    // Update the last_update_time using timer_read
    last_update_time = timer_read();
    slider_timeout_timer = last_update_time;
    uprintf("[DEBUG]: last_update_time and slider_timeout_timer set to %u\n",
            last_update_time);
}

void end_slider_visualization(void) {
    uprintf("[DEBUG]: Ending slider visualization.\n");
    // Restore previous RGB settings
    rgblight_mode_noeeprom(saved_calibration_mode);
    rgblight_sethsv_noeeprom(saved_calibration_hsv.h, saved_calibration_hsv.s, saved_calibration_hsv.v);
    rgblight_set();
    uprintf("[DEBUG]: Restored RGB mode to %d and HSV to (%d, %d, %d)\n",
            saved_calibration_mode, saved_calibration_hsv.h, saved_calibration_hsv.s, saved_calibration_hsv.v);
    slider_visualization_active = false;
}



void handle_rgb_notification(uint8_t notification_type) {
    // For future use if needed
}