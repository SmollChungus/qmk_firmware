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
extern int8_t last_moved_slider;
extern bool slider_active;

// Calibration thresholds
#define CEILING_LOW 600    // Below this is red
#define CEILING_MEDIUM 630 // Below this is orange
#define CEILING_GOOD 680   // Above this is green

// Flash settings
#define FLASH_DURATION 250
#define FLASH_COUNT 5

// via slider viz
#define SLIDER_UPDATE_DELAY 50  // 50ms delay between updates
#define TOP_ROW_LED_COUNT 16    // Number of LEDs in top row
#define SLIDER_TIMEOUT 3000  // Revert after 1 second of no updates
#define SLIDER_UPDATE_INTERVAL 20

typedef enum {
    SLIDER_TYPE_ACTUATION,
    SLIDER_TYPE_RELEASE,
    SLIDER_TYPE_RTP_DEADZONE, // Rapid Trigger Deadzone
    SLIDER_TYPE_RTP_ENGAGE,
    SLIDER_TYPE_MAX
} slider_type_t;

extern slider_type_t current_slider_type;


#define MAX_WARNING_LEDS SENSOR_COUNT

extern uint8_t saved_calibration_mode;
extern HSV saved_calibration_hsv;

void calibration_warning(void);
void flash_rgb_warning(uint8_t sensor_id, uint8_t notification_type);


typedef struct {
    uint8_t sensor_id;
    uint8_t state;
} calibration_led_t;

void start_calibration_rgb(void);
void update_calibration_rgb(uint8_t sensor_id, uint16_t ceiling);
void apply_calibration_changes_rgb(void);
void end_calibration_visual(void);

//via slider viz
void start_slider_visualization(uint8_t value);
void update_slider_visualization(uint8_t value);
void end_slider_visualization(void);
void handle_final_led_update(void);

extern bool slider_visualization_active;
extern uint16_t slider_timeout_timer;

// New global variables for final update handling
extern uint8_t latest_slider_value;
extern bool final_slider_update_pending;