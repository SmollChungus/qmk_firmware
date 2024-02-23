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

#define MATRIX_ROWS 2
#define MATRIX_COLS 2

/* Custom matrix pins and port select array delete*/

#define MUX_SEL_PINS \
    { B4, B5, B6, B7 }

/* Hardware peripherals pins */
#define CHANNELS_PER_MULTIPLEXER 16
#define APLEX_EN_PIN_0 B2
#define APLEX_EN_PIN_1 B3
#define ANALOG_PORT A3

#define DEFAULT_ACTUATION_LEVEL 650
#define DEFAULT_RELEASE_LEVEL 600
#define DEFAULT_NOISE_FLOOR_SAMPLING_COUNT 5
#define NOISE_FLOOR_THRESHOLD 30 // Example value, adjust based on your needs
#define DEFAULT_BOTTOMING_READING 1023
#define DEFAULT_CALIBRATION_STARTER true
#define BOTTOMING_CALIBRATION_THRESHOLD 100

#define EXPECTED_NOISE_FLOOR 530

#define EECONFIG_KB_DATA_SIZE 12


