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


#include "he_switch_matrix.h"
#include "analog.h"
//#include "atomic_util.h"
#include "print.h"
#include "wait.h"

const uint32_t mux_sel_pins[] = MUX_SEL_PINS; // Pins used to select MUX channel

static hesm_config_t config;

static adc_mux adcMux;

// Init function simplified for HE sensors setup
static inline void init_mux_sel(void) {
    for (int idx = 0; idx < sizeof(mux_sel_pins)/sizeof(mux_sel_pins[0]); idx++) { // not sure if this works, need more than 2 multiplexers - maybe proto col/row matrix (:
        setPinOutput(mux_sel_pins[idx]);
    }
}

/* Initialize the peripherals pins */
int hesm_init(hesm_config_t const* const hesm_config) {
    // Initialize config
    config = *hesm_config;

    adcMux = pinToMux(ANALOG_PORT);
    //Dummy call to make sure that adcStart() has been called in the appropriate state
    //adc_read(adcMux);
    init_mux_sel();

    // Enable AMUX - maybe delete? what does this even do
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


bool hesm_matrix_scan(matrix_row_t current_matrix[]) {
    bool updated = false;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t sensorId = get_sensor_id_from_row_col(row, col);
            if (sensorId != 0xFF) { // Valid sensor ID
                select_mux(sensorId);
                uint16_t sensorValue = hesm_readkey_raw(sensorId);
                // Directly update key state based on sensor value
                if (hesm_update_key(&current_matrix[row], sensorValue)) {
                    updated = true;
                    // Optionally, mark this key as changed for further processing
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
