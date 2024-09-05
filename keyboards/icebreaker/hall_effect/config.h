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

// PWM + DMA setup

#define WS2812_PWM_DRIVER PWMD1  // Timer 1 corresponds to PWMD1
#define WS2812_PWM_CHANNEL 1
#define WS2812_PWM_PAL_MODE 2
#define WS2812_PWM_DMA_STREAM STM32_DMA1_STREAM4
#define WS2812_PWM_DMA_CHANNEL 5




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




