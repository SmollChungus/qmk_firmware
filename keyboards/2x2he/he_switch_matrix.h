//he_switch_matrix.h

#pragma once

#include <stdint.h>
#include <stdbool.h>
//#include "matrix.h"
#include "gpio.h"
#include "eeconfig.h"
#include "print.h"



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
    uint16_t actuation_threshold;                  // Key press threshold
    uint16_t release_threshold;                    // Key release threshold
    uint16_t bottoming_reading[MATRIX_ROWS][MATRIX_COLS]; // Bottoming reading
    uint16_t noise_floor[MATRIX_ROWS][MATRIX_COLS];       // Detected noise floor
    bool     bottoming_calibration;                                             // calibration mode for bottoming out values (true: calibration mode, false: normal mode)
    bool     bottoming_calibration_starter[MATRIX_ROWS][MATRIX_COLS];           // calibration mode for bottoming out values (true: calibration mode, false: normal mode)
    uint16_t rescaled_actuation_threshold[MATRIX_ROWS][MATRIX_COLS];     // threshold for key press in mode 0 rescaled to actual scale
    uint16_t rescaled_release_threshold[MATRIX_ROWS][MATRIX_COLS];       // threshold for key release in mode 0 rescaled to actual scale
} he_config_t;

// For EEPROM
typedef struct PACKED {
    uint16_t actuation_threshold;
    uint16_t release_threshold;
    uint16_t bottoming_reading[MATRIX_ROWS][MATRIX_COLS];
} eeprom_he_config_t;


_Static_assert(sizeof(eeprom_he_config_t) == EECONFIG_KB_DATA_SIZE, "Mismatch in keyboard EECONFIG stored data");
extern eeprom_he_config_t eeprom_he_config;

extern he_config_t he_config;

// merge maps some time
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

//debounce
typedef struct {
    uint8_t debounced_state; // The stable state of the key
    uint8_t debounce_counter; // Counter for debouncing
} key_debounce_t;



int      he_init(void);
bool     he_matrix_scan(matrix_row_t current_matrix[]);
uint16_t he_readkey_raw(uint8_t sensorIndex);
bool     he_update_key(matrix_row_t* current_matrix, uint8_t row, uint8_t col, uint16_t sensor_value);
void     he_print_matrix(void);
extern   matrix_row_t matrix[MATRIX_ROWS];

//via slider rescale
uint16_t rescale(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);
