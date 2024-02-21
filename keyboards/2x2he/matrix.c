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

//matrix.c
#include "he_switch_matrix.h"
//#include "matrix.h"

/* matrix state(1:on, 0:off) */
matrix_row_t raw_matrix[MATRIX_ROWS]; // raw values
matrix_row_t matrix[MATRIX_ROWS];     // debounced values

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }
__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }
__attribute__((weak)) void matrix_init_user(void) {}
__attribute__((weak)) void matrix_scan_user(void) {}


void matrix_print(void) {
    hesm_print_matrix();
}

void matrix_init(void) {
    hesm_config.hesm_actuation_threshold = DEFAULT_ACTUATION_LEVEL;
    hesm_config.hesm_release_threshold   = DEFAULT_RELEASE_LEVEL;

    hesm_init(&hesm_config);

    matrix_init_kb();

    matrix_scan_kb();
}

bool matrix_scan(matrix_row_t current_matrix[]) {
    bool updated = hesm_matrix_scan();

    #ifdef CONSOLE_ENABLE
    static int cnt = 0;
    if (cnt++ == 350) {
        cnt = 0;
        hesm_print_matrix();
    }
    #endif

    return updated ? 1 : 0;
}
