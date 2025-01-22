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

#pragma once

// RGB Setup
#define WS2812_DI_PIN           A8
#define WS2812_PWM_DRIVER       PWMD1
#define WS2812_PWM_CHANNEL      1
#define WS2812_PWM_PAL_MODE     1
#define WS2812_PWM_DMA_STREAM   STM32_DMA2_STREAM5
#define WS2812_PWM_DMA_CHANNEL  6

// RGB Defaults, todo move to keyboard.json
#define RGBLIGHT_DEFAULT_MODE   RGBLIGHT_MODE_STATIC_LIGHT
#define RGBLIGHT_DEFAULT_HUE    0
#define RGBLIGHT_DEFAULT_SAT    0
#define RGBLIGHT_DEFAULT_VAL    100

//dummy, todo move to keyboard.json
#define MATRIX_ROWS 5
#define MATRIX_COLS 16

// Analog setup
#define ANALOG_PORT A3

// Multiplexer setup
#define SENSOR_COUNT    69 //68 keys + encoder click
#define MUX_EN_PINS     { B0, A7, A6, A5, A4 }
#define MUX_SEL_PINS    { B3, B4, B6, B5 }


// Memory 
#define EECONFIG_KB_DATA_SIZE       3 // eeprom_he_config
#define WEAR_LEVELING_LOGICAL_SIZE  2048
#define WEAR_LEVELING_BACKING_SIZE  16384

// Actuation Point Control config
#define EXPECTED_NOISE_FLOOR    540
#define EXPECTED_NOISE_CEILING  700
#define DEFAULT_ACTUATION_LEVEL 50
#define DEFAULT_RELEASE_LEVEL   30
#define DEBOUNCE_THRESHOLD      5

// Rapid Trigger config
#define DEFAULT_DEADZONE_RT         15
#define DEFAULT_RELEASE_DISTANCE_RT 10

// Calibration setup
#define NOISE_FLOOR_SAMPLE_COUNT 20
#define NOISE_CEILING_SAMPLE_COUNT 5

//encoder setup
#define ENCODER_CLICK_PIN_A B12
#define ENCODER_CLICK_PIN_B B15

#define FORCE_NKRO

//debug output
#define CONSOLE_VERBOSITY 1 // 0 to turn off
