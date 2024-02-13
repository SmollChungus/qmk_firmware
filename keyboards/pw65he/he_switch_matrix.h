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
//he_switch_matrix.h

#pragma once

#include <stdint.h>
#include <stdbool.h>
//#include "matrix.h"
#include "gpio.h"



#if (MATRIX_COLS <= 8)
typedef uint8_t matrix_row_t;
#elif (MATRIX_COLS <= 16)
typedef uint16_t matrix_row_t;
#elif (MATRIX_COLS <= 32)
typedef uint32_t matrix_row_t;
#else
#    error "MATRIX_COLS: invalid value"
#endif

#define MATRIX_ROW_SHIFTER ((matrix_row_t)1)

#ifdef __cplusplus
extern "C" {
#endif

/* number of matrix rows */
uint8_t matrix_rows(void);
/* number of matrix columns */
uint8_t matrix_cols(void);
/* should be called at early stage of startup before matrix_init.(optional) */
void matrix_setup(void);
/* intialize matrix for scaning. */
void matrix_init(void);
/* scan all key states on matrix */
bool matrix_scan(matrix_row_t current_matrix[]);
/* whether matrix scanning operations should be executed */
bool matrix_can_read(void);
/* whether a switch is on */
bool matrix_is_on(uint8_t row, uint8_t col);
/* matrix state on row */
matrix_row_t matrix_get_row(uint8_t row);
/* print matrix for debug */
void matrix_print(void);
/* delay between changing matrix pin state and reading values */
void matrix_output_select_delay(void);
void matrix_output_unselect_delay(uint8_t line, bool key_pressed);
/* only for backwards compatibility. delay between changing matrix pin state and reading values */
void matrix_io_delay(void);

void matrix_init_kb(void);
void matrix_scan_kb(void);

void matrix_init_user(void);
void matrix_scan_user(void);

#ifdef __cplusplus
}
#endif




typedef struct {
    uint16_t hesm_actuation_threshold;
    uint16_t hesm_release_threshold;
    uint8_t num_multiplexers;
} hesm_config_t;


typedef struct {
    uint8_t row;
    uint8_t col;
    uint8_t sensorId;
} key_to_sensor_map_t;

typedef struct {
    uint8_t sensorId;
    uint8_t muxIndex;
    uint8_t channel;
} sensor_to_mux_map_t;

hesm_config_t hesm_config;

int      hesm_init(hesm_config_t const* const hesm_config);
int      hesm_update(hesm_config_t const* const hesm_config);
bool     hesm_matrix_scan(void);
uint16_t hesm_readkey_raw(uint8_t sensorIndex);
bool     hesm_update_key(matrix_row_t* current_sensor_state, uint16_t sensorValue);
void     hesm_print_matrix(void);
extern matrix_row_t matrix[MATRIX_ROWS];
