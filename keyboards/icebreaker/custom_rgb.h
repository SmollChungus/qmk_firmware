#pragma once

#include "quantum.h"
#include "rgblight.h"

// RGB notification types
#define RGB_NOTIFICATION_CALIBRATION_WARNING 0
#define RGB_NOTIFICATION_MODE_NORMAL 1
#define RGB_NOTIFICATION_MODE_RAPID_TRIGGER 2
#define RGB_NOTIFICATION_MODE_KEY_CANCEL 3

#define LED_STATE_UNCALIBRATED 0
#define LED_STATE_PARTIAL 1
#define LED_STATE_CALIBRATED 2

// Calibration thresholds
#define CEILING_LOW 600    // Below this is red
#define CEILING_MEDIUM 630 // Below this is orange
#define CEILING_GOOD 680   // Above this is green

// Flash settings
#define FLASH_DURATION 250
#define FLASH_COUNT 5

#define MAX_WARNING_LEDS SENSOR_COUNT

void calibration_warning(void);
void flash_rgb_warning(uint8_t sensor_id, uint8_t notification_type);
void handle_rgb_notification(uint8_t notification_type);


typedef struct {
    uint8_t sensor_id;
    uint8_t state;
} calibration_led_t;

void start_calibration_rgb(void);
void update_calibration_rgb(uint8_t sensor_id, uint16_t ceiling);
void apply_calibration_changes_rgb(void);
void end_calibration_visual(void);