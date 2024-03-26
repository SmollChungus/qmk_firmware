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

#define MATRIX_ROWS 5
#define MATRIX_COLS 15

// Multiplexer setup
#define SENSOR_COUNT 66
#define MUX_EN_PINS \
    { B0, A7, A6, A5, A4 }

#define MUX_SEL_PINS \
    { B3, B4, B6, B5 }

#define ANALOG_PORT A3
#define EECONFIG_KB_DATA_SIZE 6

// User config
#define DEFAULT_ACTUATION_LEVEL 565
#define DEFAULT_RELEASE_LEVEL 560
#define DEBOUNCE_THRESHOLD 5

//Switch/sensor dependant

//Wooting Lekker
#define WOOTING_LEKKER
#ifdef WOOTING_LEKKER
#define EXPECTED_NOISE_FLOOR 512
#define EXPECTED_SWITCH_CEILING 720
#endif

// #define GEON_RAPTOR
#ifdef GEON_RAPTOR
#define EXPECTED_NOISE_FLOOR 550
#define EXPECTED_SWITCH_CEILING 950
#endif

// #define OTEMU
#ifdef OTEMU
#define EXPECTED_NOISE_FLOOR xxx
#define EXPECTED_SWITCH_CEILING xxx
#endif

// #define GEON_RAPTOR
#ifdef GATERON
#define EXPECTED_NOISE_FLOOR xxx
#define EXPECTED_SWITCH_CEILING xxx
#endif




// Calibration setup
#define NOISE_FLOOR_SAMPLE_COUNT 10




