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
#pragma once

// Corrected PWM + DMA setup

#define WS2812_PWM_DRIVER PWMD1  // Timer 1 corresponds to PWMD1
#define WS2812_PWM_CHANNEL 1
#define WS2812_PWM_PAL_MODE 1
#define WS2812_PWM_DMA_STREAM STM32_DMA2_STREAM6
#define WS2812_PWM_DMA_CHANNEL 0


// Define the pin connected to the data line of the WS2812 LEDs
#define WS2812_DI_PIN A8  // Ensure this matches the pin in your hardware


// Set maximum brightness if desired (0-255)
#define RGBLIGHT_LIMIT_VAL 120  // Adjust as needed

//debug stuff
#define CONSOLE_VERBOSITY 1 //undef to turn of
//#define DEBUG_MATRIX_SCAN_RATE


//
#define MATRIX_ROWS 5
#define MATRIX_COLS 16

// Multiplexer setup
#define SENSOR_COUNT 68
#define MUX_EN_PINS \
    { B0, A7, A6, A5, A4 }

#define MUX_SEL_PINS \
    { B3, B4, B6, B5 }

#define ANALOG_PORT A3
#define EECONFIG_KB_DATA_SIZE 4 //
#define WEAR_LEVELING_LOGICAL_SIZE 2048
#define WEAR_LEVELING_BACKING_SIZE 16384

// User config
#define DEFAULT_ACTUATION_LEVEL 50
#define DEFAULT_RELEASE_LEVEL 30
#define DEBOUNCE_THRESHOLD 5


// Rapid Trigger config
#define DEFAULT_DEADZONE_RT 15
#define DEFAULT_RELEASE_DISTANCE_RT 5

//
#define GEON_RAW_HE
#ifdef GEON_RAW_HE
#define EXPECTED_NOISE_FLOOR 540
#define EXPECTED_NOISE_CEILING 700
#endif

#define FORCE_NKRO

// Calibration setup
#define NOISE_FLOOR_SAMPLE_COUNT 10





