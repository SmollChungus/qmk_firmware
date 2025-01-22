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

#include "hall_effect.h"
#include "wait.h"
#include "print.h"
#include "rgblight.h"
#include "encoder.h"
#include "gpio.h"

matrix_row_t raw_matrix[MATRIX_ROWS];
matrix_row_t matrix[MATRIX_ROWS];

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }
__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }
__attribute__((weak)) void matrix_init_user(void) {}
__attribute__((weak)) void matrix_scan_user(void) {}

uint8_t console_output = 0; 

void matrix_print(void) {
    he_matrix_print();
}

void matrix_init(void) {
    he_init(he_key_configs, SENSOR_COUNT);

    matrix_init_kb(); // Dummy call

    noise_floor_calibration_init();

    rgblight_init();

    // Initialize encoder
    encoder_driver_init();
    setPinOutput(ENCODER_CLICK_PIN_A);
    writePinHigh(ENCODER_CLICK_PIN_A);
    palSetPadMode(GPIOB, 12, PAL_MODE_OUTPUT_PUSHPULL);
    palSetPad(GPIOB, 12);
    palSetPadMode(GPIOB, 15, PAL_MODE_INPUT_PULLDOWN);

    matrix_scan_kb(); // Dummy call
}

uint8_t matrix_scan(void) {
    int updated = he_matrix_scan();  

    if (console_output == 0) {
        /* No output */
    }
    else if (console_output == 1) { // Continuously prints the matrix state to console
        static int cnt = 0;
        if (cnt++ == 5) {
            cnt = 0;
            he_matrix_print();
        }
    }
    else if (console_output == 2) { // Continuously prints debug for (0,0)
        he_matrix_print_rapid_trigger_debug();
    }

    encoder_driver_task(); 

    matrix_scan_kb(); // Dummy call

    return updated;
}