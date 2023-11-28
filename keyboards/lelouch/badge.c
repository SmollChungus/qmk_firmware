// Include necessary files
#include "quantum.h"
#include "rgblight.h"
#include "color.h"

// Define custom channel ID
#define id_custom_channel 0
#define NUM_LEDS 37  // Adjust the value according to your actual number of LEDs

// Define custom RGB values
enum via_rgb_value {
    id_custom_rgblight_brightness = 1,  // Start from 1
    id_custom_rgblight_color = 2,
};

// LED_TYPE should be declared once, not twice
// extern LED_TYPE rgb_led[NUM_LEDS];
LED_TYPE rgb_led[NUM_LEDS];

// Global variables to store RGB settings
static uint8_t badge_rgblight_brightness = 0;
static uint8_t badge_rgblight_color = 0;

// Declare the data array globally
uint8_t data[6];  // Assuming the data array has at least 6 elements

// Function declarations for communication
void handle_custom_set_value(uint8_t *data);
void handle_custom_get_value(uint8_t *data);
void handle_custom_save(void);
void update_custom_rgblight_color(uint8_t value);
void update_custom_rgblight_brightness(uint8_t value, uint8_t *brightness);

// Implement via_custom_value_command_kb
void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id = &(data[0]);
    uint8_t *channel_id = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    // Debugging: Print received data
    printf("Received VIA data - Command ID: %d, Channel ID: %d, Value ID/Data: %d\n", *command_id, *channel_id, *value_id_and_data);

    if (*channel_id == id_custom_channel) {
        switch (*command_id) {
            case id_custom_set_value:
                // Debugging: Print information about handling custom set value command
                printf("Handling custom set value command\n");
                handle_custom_set_value(value_id_and_data);
                break;
            case id_custom_get_value:
                // Debugging: Print information about handling custom get value command
                printf("Handling custom get value command\n");
                handle_custom_get_value(value_id_and_data);
                break;
            case id_custom_save:
                // Debugging: Print information about handling custom save command
                printf("Handling custom save command\n");
                handle_custom_save();
                break;
            default:
                *command_id = id_unhandled;
                // Debugging: Print information about unhandled command
                printf("Unhandled command\n");
                break;
        }
        return;
    }

    // Debugging: Print information about handling other channels
    printf("Handling other channels\n");

    // Handle other channels if needed
    *command_id = id_unhandled;
}

void handle_custom_set_value(uint8_t *data) {
    uint8_t *value_id = &(data[0]);
    uint8_t *value_data = &(data[1]);

    // Debugging: Print information about handling custom set value command
    printf("Handling custom set value command - Value ID: %d, Value Data: %d\n", *value_id, *value_data);

    switch (*value_id) {
        case id_custom_rgblight_brightness:
            // Debugging: Print information about updating custom RGB brightness
            printf("Updating custom RGB brightness to: %d\n", *value_data);
            update_custom_rgblight_brightness(*value_data, &badge_rgblight_brightness);
            break;
        case id_custom_rgblight_color:
            // Debugging: Print information about updating custom RGB color
            printf("Updating custom RGB color to: %d\n", *value_data);
            update_custom_rgblight_color(*value_data);
            break;
        default:
            // Debugging: Print information about unknown value IDs
            printf("Unknown value ID: %d\n", *value_id);
            // Handle unknown value IDs or ignore them
            break;
    }
}

void handle_custom_get_value(uint8_t *data) {
    uint8_t *value_id = &(data[0]);
    uint8_t *value_data = &(data[1]);

    // Debugging: Print information about handling custom get value command
    printf("Handling custom get value command\n");

    switch (*value_id) {
        case id_custom_rgblight_brightness:
            // Debugging: Print information about sending custom RGB brightness
            printf("Sending custom RGB brightness: %d\n", badge_rgblight_brightness);
            *value_data = badge_rgblight_brightness;
            break;
        case id_custom_rgblight_color:
            // Debugging: Print information about sending custom RGB color
            printf("Sending custom RGB color: %d\n", badge_rgblight_color);
            *value_data = badge_rgblight_color;
            break;
        default:
            // Debugging: Print information about unknown value IDs
            printf("Unknown value ID: %d\n", *value_id);
            // Handle unknown value IDs or ignore them
            break;
    }
}

void handle_custom_save(void) {
    // Debugging: Print information about handling custom save command
    printf("Handling custom save command\n");

    // Implement saving values to EEPROM for the custom RGB LED group if needed
    // For now, add a log statement to indicate that saving is not implemented
    printf("Saving custom values is not implemented\n");
}

// Assuming that VIA provides hue and saturation values in the data
// Adjust the indices accordingly based on your VIA protocol

// Example: data = [ command_id, channel_id, value_id, hue, saturation, brightness ]
void set_badge_rgb_brightness(uint8_t brightness) {
    // Debugging: Print information about setting badge RGB brightness
    printf("Setting badge RGB brightness to: %d\n", brightness);

    // Convert HSV to RGB using the received values
    HSV hsv_color;
    hsv_color.h = data[3];  // Assuming hue is at index 3
    hsv_color.s = data[4];  // Assuming saturation is at index 4
    hsv_color.v = brightness;
    RGB rgb_color = hsv_to_rgb_nocie(hsv_color);

    // Set the RGB values of the last 3 LEDs in the LED chain
    for (int i = NUM_LEDS - 3; i < NUM_LEDS; ++i) {
        rgb_led[i].r = rgb_color.r;
        rgb_led[i].g = rgb_color.g;
        rgb_led[i].b = rgb_color.b;
    }

    // Update or refresh the RGB lighting
    rgblight_set();
}

void update_custom_rgblight_brightness(uint8_t value, uint8_t *brightness) {
    // Debugging: Print information about updating custom RGB brightness
    printf("Updating custom RGB brightness to: %d\n", value);

    // Update the custom RGB brightness
    *brightness = value;

    // Update badge RGB brightness
    badge_rgblight_brightness = value;

    // Add code to set your RGB hardware brightness here
    // The actual implementation depends on your RGB hardware
    // You might want to have separate functions for backlight and badge brightness
    set_badge_rgb_brightness(badge_rgblight_brightness);
}

void update_custom_rgblight_color(uint8_t hue) {
    // Calculate saturation and value based on brightness (adjust these values as needed)
    uint8_t saturation = 255;  // You may adjust this based on your requirements
    uint8_t value = badge_rgblight_brightness;  // Use the brightness value you stored

    // Print the color values for debugging
    printf("Setting custom color - H: %d, S: %d, V: %d\n", hue, saturation, value);

    // Loop through the last 3 LEDs and set their RGB hardware color using QMK's rgblight_sethsv function
    for (int i = NUM_LEDS - 3; i < NUM_LEDS; ++i) {
        rgb_led[i].r = hue;
        rgb_led[i].g = saturation;
        rgb_led[i].b = value;
    }

    // Update badge_rgblight_color
    badge_rgblight_color = hue;

    // Update or refresh the RGB lighting for the last 3 LEDs
    rgblight_set();
}
