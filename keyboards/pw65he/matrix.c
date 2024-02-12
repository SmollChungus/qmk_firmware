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
#include "matrix.h"



const key_to_sensor_map_t key_to_sensor_map[] = {
    // Row 0
    {0,0,0},{0,1,1},{0,2,2},{0,3,3},{0,4,4},{0,5,5},{0,6,6},{0,7,7},{0,8,8},{0,9,9},{0,10,10},{0,11,11},{0,12,12},{0,13,13},{0,14,14}, // sensor 14 is missing, sorry takes 2, Delete is the next
    // Row 1
    {1,0,15},{1,1,16},{1,2,17},{1,3,18},{1,4,19},{1,5,20},{1,6,21},{1,7,22},{1,8,23},{1,9,24},{1,10,25},{1,11,26},{1,12,27},{1,13,28},{1,14,63}, // "|" takes 1.5, Insert is next
    // Row 2
    {2,0,29},{2,1,30},{2,2,31},{2,3,32},{2,4,33},{2,5,34},{2,6,35},{2,7,36},{2,8,37},{2,9,38},{2,10,39},{2,11,41},{2,12,64},{2,13,65}, // Enter takes 2.25, Home is next
    // Row 3
    {3,0,42},{3,1,43},{3,2,44},{3,3,45},{3,4,46},{3,5,47},{3,6,48},{3,7,49},{3,8,50},{3,9,51},{3,10,52},{3,11,53},{3,12,66}, // Shift takes 2.25, ↑ and End follow
    // Row 4
    {4,0,54},{4,1,55},{4,2,56},{4,3,57},{4,4,58},{4,5,59},{4,6,60},{4,7,61},{4,8,62}
};




const sensor_to_mux_map_t sensor_to_mux_map[] = {
    {0,0,0},{1,0,1},{2,0,2},{3,0,3},{4,0,4},{5,0,5},{6,0,6},{7,0,7},{8,0,8},{9,0,9},{10,0,10},{11,0,11},{12,0,12},{13,0,13},{14,0,14},
    // Sensor ID 14 is missing -  does this break anything?
    {15,0,15},{16,1,0},{17,1,1},{18,1,2},{19,1,3},{20,1,4},{21,1,5},{22,1,6},{23,1,7},{24,1,8},{25,1,9},{26,1,10},{27,1,11},{28,1,12},
    {29,1,13},{30,1,14},{31,1,15},{32,2,0},{33,2,1},{34,2,2},{35,2,3},{36,2,4},{37,2,5},{38,2,6},{39,2,7},{40,2,8},{41,2,9},
    {42,2,10},{43,2,11},{44,2,12},{45,2,13},{46,2,14},{47,2,15},{48,3,0},{49,3,1},{50,3,2},{51,3,3},{52,3,4},{53,3,5},
    {54,3,6},{55,3,7},{56,3,8},{57,3,9},{58,3,10},{59,3,11},{60,3,12},{61,3,13},{62,3,14},{63,3,15},{64,4,0},{65,4,1},{66,4,2}
};


/* matrix state(1:on, 0:off) */
extern matrix_row_t raw_matrix[MATRIX_ROWS]; // raw values
extern matrix_row_t matrix[MATRIX_ROWS];     // debounced values

void matrix_init_custom(void) {
    // Default values, overwritten by VIA if enabled later
    hesm_config.hesm_actuation_threshold = DEFAULT_ACTUATION_LEVEL;
    hesm_config.hesm_release_threshold   = DEFAULT_RELEASE_LEVEL;

    hesm_init(&hesm_config);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool updated = hesm_matrix_scan(current_matrix);



// RAW matrix values on console
#ifdef CONSOLE_ENABLE
    static int cnt = 0;
    if (cnt++ == 350) {
        cnt = 0;
        hesm_print_matrix();
    }
#endif
    return updated;
}
