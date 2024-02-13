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
//he_switch_matrix.c

#include "he_switch_matrix.h"
#include "analog.h"
//#include "atomic_util.h"
#include "print.h"
#include "wait.h"

const key_to_sensor_map_t key_to_sensor_map[] = {
    // Row 0
    {0,0,0},{0,1,1},{0,2,2},{0,3,3},{0,4,4},{0,5,5},{0,6,6},{0,7,7},{0,8,8},{0,9,9},{0,10,10},{0,11,11},{0,12,12},{0,13,13},{0,14,14},
    // Row 1
    {1,0,15},{1,1,16},{1,2,17},{1,3,18},{1,4,19},{1,5,20},{1,6,21},{1,7,22},{1,8,23},{1,9,24},{1,10,25},{1,11,26},{1,12,27},{1,13,28},{1,14,63},
    // Row 2
    {2,0,29},{2,1,30},{2,2,31},{2,3,32},{2,4,33},{2,5,34},{2,6,35},{2,7,36},{2,8,37},{2,9,38},{2,10,39},{2,11,41},{2,12,64},{2,13,65},
    // Row 3
    {3,0,42},{3,1,43},{3,2,44},{3,3,45},{3,4,46},{3,5,47},{3,6,48},{3,7,49},{3,8,50},{3,9,51},{3,10,52},{3,11,53},{3,12,66},
    // Row 4
    {4,0,54},{4,1,55},{4,2,56},{4,3,57},{4,4,58},{4,5,59},{4,6,60},{4,7,61},{4,8,62}
};

//id,muxId,channelId
const sensor_to_mux_map_t sensor_to_mux_map[] = {
    {0,0,0},{1,0,1},{2,0,2},{3,0,3},{4,0,4},{5,0,5},{6,0,6},{7,0,7},{8,0,8},{9,0,9},{10,0,10},{11,0,11},{12,0,12},{13,0,13},{14,0,14},
    {15,0,15},{16,1,0},{17,1,1},{18,1,2},{19,1,3},{20,1,4},{21,1,5},{22,1,6},{23,1,7},{24,1,8},{25,1,9},{26,1,10},{27,1,11},{28,1,12},
    {29,1,13},{30,1,14},{31,1,15},{32,2,0},{33,2,1},{34,2,2},{35,2,3},{36,2,4},{37,2,5},{38,2,6},{39,2,7},{40,2,8},{41,2,9},
    {42,2,10},{43,2,11},{44,2,12},{45,2,13},{46,2,14},{47,2,15},{48,3,0},{49,3,1},{50,3,2},{51,3,3},{52,3,4},{53,3,5},
    {54,3,6},{55,3,7},{56,3,8},{57,3,9},{58,3,10},{59,3,11},{60,3,12},{61,3,13},{62,3,14},{63,3,15},{64,4,0},{65,4,1},{66,4,2}
};

matrix_row_t matrix_get_row(uint8_t row) {
    if (row < MATRIX_ROWS) {
        return matrix[row];
    } else {
        return 0; // Return an empty row if out of bounds
    }
}

const uint32_t mux_sel_pins[] = MUX_SEL_PINS;

static hesm_config_t config;

static adc_mux adcMux;

// Init function simplified for HE sensors setup
static inline void init_mux_sel(void) {
    for (int idx = 0; idx < sizeof(mux_sel_pins)/sizeof(mux_sel_pins[0]); idx++) {
        setPinOutput(mux_sel_pins[idx]);
    }
}

/* Initialize the peripherals pins */
int hesm_init(hesm_config_t const* const hesm_config) {
    config = *hesm_config;

    adcMux = pinToMux(ANALOG_PORT);
    adc_read(adcMux);

    init_mux_sel();

    setPinOutput(APLEX_EN_PIN_0);
    writePinLow(APLEX_EN_PIN_0);
    setPinOutput(APLEX_EN_PIN_1);
    writePinLow(APLEX_EN_PIN_1);

    return 0;
}

static inline void select_mux(uint8_t sensorId) {
    // Look up the multiplexer and channel directly from the sensor ID
    for (int i = 0; i < sizeof(sensor_to_mux_map) / sizeof(sensor_to_mux_map[0]); i++) {
        if (sensor_to_mux_map[i].sensorId == sensorId) {
            uint8_t muxIndex = sensor_to_mux_map[i].muxIndex;
            uint8_t channel = sensor_to_mux_map[i].channel;

            // Deactivate all multiplexers first - loop over aplex voor scalability?
            writePinLow(APLEX_EN_PIN_0);
            writePinLow(APLEX_EN_PIN_1);
            // Activate the correct multiplexer
            if (muxIndex == 0) {
                writePinHigh(APLEX_EN_PIN_0);
            } else if (muxIndex == 1) {
                writePinHigh(APLEX_EN_PIN_1);
            }
            // Set the correct channel on the activated multiplexer
            // This part depends on your specific multiplexer selection mechanism
            // Example for a 4-channel selection:
            for (int j = 0; j < 4; j++) {
                if (channel & (1 << j)) {
                    writePinHigh(mux_sel_pins[j]);
                } else {
                    writePinLow(mux_sel_pins[j]);
                }
            }
            break; // Exit the loop once the correct multiplexer is selected
        }
    }
}
// Assuming key_to_sensor_map is defined similar to sensor_to_mux_map
uint8_t get_sensor_id_from_row_col(uint8_t row, uint8_t col) {
    for (int i = 0; i < (sizeof(key_to_sensor_map) / sizeof(key_to_sensor_map[0])); i++) {
        if (key_to_sensor_map[i].row == row && key_to_sensor_map[i].col == col) {
            return key_to_sensor_map[i].sensorId;
        }
    }
    return 0xFF; // Return an invalid ID if not found
}


 // why is this deleted in gtp4?
int hesm_update(hesm_config_t const* const hesm_config) {
    // Save config
    config = *hesm_config;
    return 0;
}

// Read the HE sensor value - replace matrix with direct pin
// Function to read HE sensor value directly through MUX and ADC
uint16_t hesm_readkey_raw(uint8_t sensorIndex) {
    uint16_t sensorValue = 0;

    select_mux(sensorIndex);
    sensorValue = adc_read(adcMux); // Read the ADC value for the selected sensor
    return sensorValue; // Return the sensor value
}

// Update press/release state of a single key # CURRENT_SENSOR_VALUE SHOULD BE sw_value right?
bool hesm_update_key(matrix_row_t* current_sensor_state, uint16_t sensorValue) {
    bool current_state = *current_sensor_state & 1; // Assuming current_sensor_state tracks the state of a single sensor

    // Determine the new state based on sensorValue and thresholds
    if (current_state && sensorValue < config.hesm_actuation_threshold) {
        *current_sensor_state = 0; // Key released
        return true;
    } else if (!current_state && sensorValue > config.hesm_release_threshold) {
        *current_sensor_state = 1; // Key pressed
        return true;
    }

    return false; // No change in state
}


bool hesm_matrix_scan(void) {
    bool updated = false;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t sensorId = get_sensor_id_from_row_col(row, col);
            if (sensorId != 0xFF) { // Valid sensor ID
                select_mux(sensorId);
                uint16_t sensorValue = hesm_readkey_raw(sensorId);
                // Assume matrix is a global matrix state accessible here
                if (hesm_update_key(&matrix[row], sensorValue)) {
                    updated = true;
                }
            }
        }
    }
    return updated;
}




// Debug print key values
void hesm_print_matrix(void) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            uprintf("test");
            if (col < (MATRIX_COLS - 1)) {
                print(",");
            }
        }
        print("\n");
    }
    print("\n");
}
