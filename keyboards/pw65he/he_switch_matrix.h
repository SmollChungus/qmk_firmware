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

// he_switch_matrix.h
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "matrix.h"

typedef struct {
    uint16_t hesm_actuation_threshold;
    uint16_t hesm_release_threshold;
    uint8_t num_multiplexers; // New field to store the number of multiplexers
} hesm_config_t;


typedef struct {
    uint8_t row;
    uint8_t col;
    uint8_t sensorId;
} key_to_sensor_map_t;

typedef struct {
    uint8_t sensorId;
    uint8_t muxIndex; // 0 for AMUX0, 1 for AMUX1, etc.
    uint8_t channel;  // Channel on the specified multiplexer
} sensor_to_mux_map_t;

hesm_config_t hesm_config;

int      hesm_init(hesm_config_t const* const hesm_config);
int      hesm_update(hesm_config_t const* const hesm_config);
bool     hesm_matrix_scan(matrix_row_t current_matrix[]);
uint16_t hesm_readkey_raw(uint8_t sensorIndex);
bool     hesm_update_key(matrix_row_t* current_sensor_state, uint16_t sensorValue);
void     hesm_print_matrix(void);
