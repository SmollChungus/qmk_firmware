//he_switch_matrix.h

#pragma once

#include <stdint.h>
#include <stdbool.h>
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

uint8_t matrix_rows(void);
uint8_t matrix_cols(void);
bool matrix_scan(matrix_row_t current_matrix[]);
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
matrix_row_t matrix_get_row(uint8_t row);

#ifdef __cplusplus
}
#endif




typedef struct {
    uint16_t he_actuation_threshold;
    uint16_t he_release_threshold;
    uint8_t num_multiplexers; //yeet
} he_config_t;

typedef struct {
    uint16_t he_actuation_threshold;
    uint16_t he_release_threshold;
    // Add any future persistent settings here
} eeprom_he_config_t;


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

//db
typedef struct {
    uint8_t debounced_state; // The stable state of the key
    uint8_t debounce_counter; // Counter for debouncing
} key_debounce_t;

he_config_t he_config;

int      he_init(he_config_t const* const he_config);
int      he_update(he_config_t const* const he_config);
bool     he_matrix_scan(void);
uint16_t he_readkey_raw(uint8_t sensorIndex);
bool he_update_key(matrix_row_t* current_matrix, uint8_t row, uint8_t col, uint16_t sensor_value);
void     he_print_matrix(void);
extern matrix_row_t matrix[MATRIX_ROWS];
