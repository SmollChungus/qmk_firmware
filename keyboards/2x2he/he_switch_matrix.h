//he_switch_matrix.h

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
__attribute__((weak)) void keyboard_post_init_user(void);
matrix_row_t matrix_get_row(uint8_t row);

#ifdef __cplusplus
}
#endif

typedef struct {
    uint16_t he_actuation_threshold;
    uint16_t he_release_threshold;
} he_config_t;

typedef struct {
    uint16_t he_actuation_threshold;
    uint16_t he_release_threshold;
} eeprom_he_config_t;

typedef struct {
    uint16_t he_actuation_threshold;
    uint16_t he_release_threshold;
} via_he_config_t;

typedef struct {
    uint8_t row;
    uint8_t col;
    uint8_t sensor_id;
    uint8_t mux_id;
    uint8_t mux_channel;
} sensor_to_matrix_map_t;

//debounce
typedef struct {
    uint8_t debounced_state; // The stable state of the key
    uint8_t debounce_counter; // Counter for debouncing
} key_debounce_t;

extern he_config_t he_config;
extern eeprom_he_config_t eeprom_he_config;
extern via_he_config_t via_he_config;

_Static_assert(sizeof(eeprom_he_config_t) == EECONFIG_KB_DATA_SIZE, "Mismatch in keyboard EECONFIG stored data");

int      he_init(he_config_t const* const he_config);
bool     he_matrix_scan(void);
uint16_t he_readkey_raw(uint8_t sensorIndex);
bool     he_update_key(matrix_row_t* current_matrix, uint8_t row, uint8_t col, uint16_t sensor_value);
void     he_matrix_print(void);
void     via_update_config(void);
extern   matrix_row_t matrix[MATRIX_ROWS];
