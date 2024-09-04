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
//config.h
#pragma once

//debug stuff
#define CONSOLE_VERBOSITY 1 //undef to turn of
#define DEBUG_MATRIX_SCAN_RATE


//#define matrix_shenanigans

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
#define EECONFIG_KB_DATA_SIZE 5 //
#define WEAR_LEVELING_LOGICAL_SIZE 2048
#define WEAR_LEVELING_BACKING_SIZE 16384

// User config
#define DEFAULT_ACTUATION_LEVEL 575
#define DEFAULT_RELEASE_LEVEL 565
#define DEBOUNCE_THRESHOLD 5


// Rapid Trigger config
#define DEFAULT_DEADZONE_RT 565
#define DEFAULT_RELEASE_DISTANCE_RT 20

//
#define GEON_RAW_HE
#ifdef GEON_RAW_HE
#define EXPECTED_NOISE_FLOOR 540
#define EXPECTED_NOISE_CEILING 700
#endif

#define FORCE_NKRO

// Calibration setup
#define NOISE_FLOOR_SAMPLE_COUNT 10

#ifdef RAW_ENABLE
#define MATRIX_STATE_REPORT_ID 0x01
#define SENSOR_VALUE_REPORT_ID_BASE 0x02 // Base for sensor value reports
#define MATRIX_STATE_REPORT_SIZE 32
#define SENSOR_REPORT_SIZE 32
#define SENSORS_PER_REPORT 15
#define REPORT_INTERVAL_MS 500
#define NUM_SENSOR_REPORTS 5
#endif



//rgb 
#define RGB_MATRIX_LED_COUNT 68


#define RGB_DISABLE_WHEN_USB_SUSPENDED

#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 120

#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_CYCLE_LEFT_RIGHT

#define ENABLE_RGB_MATRIX_ALPHAS_MODS
#define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_BREATHING
#define ENABLE_RGB_MATRIX_BAND_SAT
#define ENABLE_RGB_MATRIX_BAND_VAL
#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
#define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
#define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT
#define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
#define ENABLE_RGB_MATRIX_CYCLE_ALL
#define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT
#define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
#define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
#define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
#define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
#define ENABLE_RGB_MATRIX_DUAL_BEACON
#define ENABLE_RGB_MATRIX_RAINBOW_BEACON
#define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
#define ENABLE_RGB_MATRIX_RAINDROPS
#define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
#define ENABLE_RGB_MATRIX_HUE_BREATHING
#define ENABLE_RGB_MATRIX_HUE_PENDULUM
#define ENABLE_RGB_MATRIX_HUE_WAVE
#define ENABLE_RGB_MATRIX_PIXEL_FRACTAL
#define ENABLE_RGB_MATRIX_PIXEL_FLOW
#define ENABLE_RGB_MATRIX_PIXEL_RAIN
