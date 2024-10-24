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
    for (int i = 0; i < SENSOR_COUNT; i++) {
        calibration_leds[i].sensor_id = i;
        calibration_leds[i].state = LED_STATE_UNCALIBRATED;
    }
    apply_calibration_changes_rgb();
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
    // Restore default RGB settings
    rgblight_reload_from_eeprom();
}


void handle_rgb_notification(uint8_t notification_type) {
    // For future use if needed
}