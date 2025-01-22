/* Copyright 2025 Matthijs Muller
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "eeprom.h"
#include "eeconfig.h"

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

uint8_t matrix_rows(void);
uint8_t matrix_cols(void);
matrix_row_t matrix_get_row(uint8_t row);
uint8_t matrix_scan(void);
bool matrix_can_read(void);
bool matrix_is_on(uint8_t row, uint8_t col);
void matrix_print(void);
void matrix_output_select_delay(void);
void matrix_output_unselect_delay(uint8_t line, bool key_pressed);
void matrix_setup(void);
void matrix_init(void);
void matrix_io_delay(void);
void matrix_init_kb(void);
void matrix_scan_kb(void);
void matrix_init_user(void);
void matrix_scan_user(void);
__attribute__((weak)) void keyboard_post_init_user(void);

#ifdef __cplusplus
}
#endif

// todo merge structs
typedef struct {
    bool    he_calibration_mode;
    bool    he_post_flash_flag;
    uint8_t he_actuation_mode;
} he_config_t;

typedef struct {
    bool    he_calibration_mode;
    bool    he_post_flash_flag;
    uint8_t he_actuation_mode;
} eeprom_he_config_t;

// Redundancy to decouple incoming VIA commands from runtime/EEPROM
typedef struct {
    bool    he_calibration_mode;
    bool    he_post_flash_flag;
    uint8_t he_actuation_mode;
} via_he_config_t;

// Stored per key for future use
typedef struct {
    uint8_t he_actuation_threshold;
    uint8_t he_release_threshold;
    uint16_t noise_floor;
    uint16_t noise_ceiling;
} he_key_config_t;

typedef struct {
    uint8_t deadzone;
    uint8_t rt_actuation_point;
    uint8_t boundary_value;
    uint8_t engage_distance;
    uint8_t disengage_distance;
} he_key_rapid_trigger_config_t;

typedef struct {
    uint8_t deadzone;
    uint8_t rt_actuation_point;
    uint8_t boundary_value;
    uint8_t engage_distance;
    uint8_t disengage_distance;
} eeprom_he_key_rapid_trigger_config_t;

typedef struct {
    uint8_t he_actuation_threshold;
    uint8_t he_release_threshold;
    uint16_t noise_floor;
    uint16_t noise_ceiling;
} eeprom_he_key_config_t;

typedef struct {
    uint8_t he_actuation_threshold;
    uint8_t he_release_threshold;
    uint16_t noise_floor;
    uint16_t noise_ceiling;
} via_he_key_config_t;

typedef struct {
    uint8_t row;
    uint8_t col;
    uint8_t sensor_id;
    uint8_t mux_id;
    uint8_t mux_channel;
} sensor_to_matrix_map_t;

typedef struct {
    uint8_t debounced_state;
    uint8_t debounce_counter;
} key_debounce_t;


extern he_config_t he_config;
extern eeprom_he_config_t eeprom_he_config;
extern via_he_config_t via_he_config;
extern he_key_config_t he_key_configs[SENSOR_COUNT];
extern eeprom_he_key_config_t eeprom_he_key_configs[SENSOR_COUNT];
extern via_he_key_config_t via_he_key_configs[SENSOR_COUNT];
extern he_key_rapid_trigger_config_t he_key_rapid_trigger_configs[SENSOR_COUNT];
extern eeprom_he_key_rapid_trigger_config_t eeprom_he_key_rapid_trigger_configs[SENSOR_COUNT];

#define ACTUATION_MODE_NORMAL 0
#define ACTUATION_MODE_RAPID_TRIGGER 1
#define ACTUATION_MODE_KEY_CANCEL 2

extern uint16_t eeprom_save_timer;
extern bool eeprom_save_pending;
#define EEPROM_SAVE_DELAY 2000

_Static_assert(sizeof(eeprom_he_config) == EECONFIG_KB_DATA_SIZE, "Mismatch in EECONFIG_KB_DATA_SIZE");
_Static_assert(sizeof(eeprom_he_key_configs)  == EECONFIG_USER_DATA_SIZE, "Mismatch in EECONFIG_USER_DATA_SIZE");


int       he_init(he_key_config_t he_key_configs[], size_t count);
int       compare_uint16(const void *a, const void *b);
uint8_t   sensor_value_rescaled;
uint16_t  he_readkey_raw(uint8_t sensorIndex);
uint16_t  noise_floor[SENSOR_COUNT];
bool      he_matrix_scan(void);
bool      he_update_key(matrix_row_t* current_matrix, uint8_t row, uint8_t col,uint8_t sensor_id, uint16_t sensor_value);
bool      he_update_key_keycancel(matrix_row_t* current_matrix, uint8_t row, uint8_t col, uint8_t sensor_id, uint16_t sensor_value);
bool      he_update_key_rapid_trigger(matrix_row_t* current_matrix, uint8_t row, uint8_t col,uint8_t sensor_id, uint16_t sensor_value);
bool      he_update_encoder_handle(matrix_row_t* current_matrix, uint8_t row, uint8_t col, uint8_t sensor_id, uint16_t sensor_value);
void      noise_ceiling_calibration(void);
void      he_matrix_print(void);
void      he_matrix_print_extended(void);
void      he_matrix_print_rapid_trigger(void);
void      he_matrix_print_rapid_trigger_debug(void);
void      via_update_config(void);
extern    matrix_row_t matrix[MATRIX_ROWS];
void      send_matrix_state_report(void);
void      send_sensor_value_report(uint8_t report_number, uint8_t start_sensor);
void      noise_floor_calibration_init(void);
void      noise_floor_calibration(void);
void      save_calibration_data_to_eeprom(void);
void      via_he_calibration_save(void);
void      via_he_config_send_value(uint8_t value_id, uint16_t value);
void      via_he_config_get_value(uint8_t *data);

#define DEBUG_SAMPLE_COUNT 15

// Statistics and debug
typedef struct {
    uint16_t samples[DEBUG_SAMPLE_COUNT];
    uint8_t index;
} sensor_data_t;
