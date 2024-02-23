/* he_switch_matrix.c - #include "matrix.h" tasks are delegated to he_switch_matrix.*/
#include "he_switch_matrix.h"
#include "analog.h"
//#include "atomic_util.h" no need so far, needs testing
#include "print.h"
#include "wait.h"
#include "math.h"


eeprom_he_config_t eeprom_he_config;
he_config_t he_config;
static key_debounce_t debounce_matrix[MATRIX_ROWS][MATRIX_COLS] = {{{0, 0}}};
void he_noise_floor(void);

//MAPS
// map = row, col, sensor_id
const key_to_sensor_map_t key_to_sensor_map[] = {
    {0,0,0}, {0,1,1},
    {1,0,2}, {1,1,3}
};

// map = sensor_id, mux_id, mux_channel
const sensor_to_mux_map_t sensor_to_mux_map[] = {
    {0,0,4},  {1,0,3},
    {2,0,12},  {3,0,13}
};

matrix_row_t matrix_get_row(uint8_t row) {
    if (row < MATRIX_ROWS) {
        return matrix[row];
    } else {
        return 0; // Return an empty row if out of bounds
    }
}



const uint32_t mux_sel_pins[] = MUX_SEL_PINS;

static adc_mux adcMux;

// Init function simplified for HE sensors setup
static inline void init_mux_sel(void) {
    for (int idx = 0; idx < sizeof(mux_sel_pins)/sizeof(mux_sel_pins[0]); idx++) {
        setPinOutput(mux_sel_pins[idx]);
    }
}

/* Initialize the peripherals pins */
int he_init(void) {
    init_mux_sel();
    palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
    adcMux = pinToMux(ANALOG_PORT);

    // Initialize from EEPROM if available, otherwise use defaults
    if (eeconfig_is_enabled()) {
        // Read the block of data from EEPROM into he_config
        eeprom_read_block((void*)&he_config, (const void*)&eeprom_he_config, sizeof(he_config_t));
    } else {
        // EEPROM not set up, use default values
        he_config.actuation_threshold = DEFAULT_ACTUATION_LEVEL;
        he_config.release_threshold = DEFAULT_RELEASE_LEVEL;
        // Potentially set other default values here

        // Save the default config to EEPROM
        eeprom_update_block((const void*)&he_config, (void*)&eeprom_he_config, sizeof(he_config_t));
    }
    adc_read(adcMux);

    setPinOutput(APLEX_EN_PIN_0);
    writePinLow(APLEX_EN_PIN_0);
    setPinOutput(APLEX_EN_PIN_1);
    writePinLow(APLEX_EN_PIN_1);

    he_noise_floor();


    return 0;
}


static inline void select_mux(uint8_t sensorId) {
    // Look up the multiplexer and channel directly from the sensor ID
    for (int i = 0; i < sizeof(sensor_to_mux_map) / sizeof(sensor_to_mux_map[0]); i++) {
        if (sensor_to_mux_map[i].sensorId == sensorId) {
            uint8_t muxIndex = sensor_to_mux_map[i].muxIndex;
            uint8_t channel = sensor_to_mux_map[i].channel;

            // Deactivate all multiplexers first - loop over aplex voor scalability? just leave multiplexers on kek?
            writePinLow(APLEX_EN_PIN_0);
            writePinLow(APLEX_EN_PIN_1);
            // Activate the correct multiplexer
            if (muxIndex == 0) {
                writePinHigh(APLEX_EN_PIN_0);
            } else if (muxIndex == 1) {
                writePinHigh(APLEX_EN_PIN_1);
            }
            // mux 4-channel selection:
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



// Read the HE sensor value - replace matrix with direct pin
// Function to read HE sensor value directly through MUX and ADC
uint16_t he_readkey_raw(uint8_t sensorIndex) {
    uint16_t sensor_value = 0;

    select_mux(sensorIndex);
    sensor_value = adc_read(adcMux); // Read the ADC value for the selected sensor
    return sensor_value; // Return the sensor value
}




// Update press/release state of a single key # CURRENT_SENSOR_VALUE SHOULD BE sw_value right?
// Assume row and col are available and correctly identify the key's position
bool he_update_key(matrix_row_t* current_matrix, uint8_t row, uint8_t col, uint16_t sensor_value) {
    bool new_state = sensor_value > he_config.actuation_threshold;
    key_debounce_t *key_info = &debounce_matrix[row][col];

    if (new_state != key_info->debounced_state) {
        // If the new state is different, increment the debounce counter
        if (++key_info->debounce_counter >= DEBOUNCE_THRESHOLD) {
            // If the debounce threshold is reached, update the debounced state
            key_info->debounced_state = new_state;
            key_info->debounce_counter = 0;

            // Update the actual matrix state
            if (new_state) {
                current_matrix[row] |= (1UL << col);
            } else {
                current_matrix[row] &= ~(1UL << col);
            }

            return true; // State changed
        }
    } else {
        // If the state is the same as the debounced state, reset the counter
        key_info->debounce_counter = 0;
    }

    return false; // No change in stable state
}

// Get the noise floor
void he_noise_floor(void) {
    // Reset the noise floor values to expected
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            he_config.noise_floor[row][col] = EXPECTED_NOISE_FLOOR;
            uprintf("Expected Noise Floor: %d", he_config.noise_floor[row][col]);
        }
    }

    // Take multiple samples to average out the noise floor
    for (uint8_t i = 0; i < DEFAULT_NOISE_FLOOR_SAMPLING_COUNT; i++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
                uint8_t sensorId = get_sensor_id_from_row_col(row, col);
                if (sensorId != 0xFF) { // Check if valid sensor ID
                    select_mux(sensorId);
                    uint8_t rawValue = he_readkey_raw(sensorId);
                    uprintf("Raw Value: %d", rawValue);
                    he_config.noise_floor[row][col] += rawValue;
                }
            }
        }
        wait_ms(5); // Wait a bit between samples
    }

    // Calculate the average noise floor
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            he_config.noise_floor[row][col] /= DEFAULT_NOISE_FLOOR_SAMPLING_COUNT;
            uprintf("Average Noise Floor: %d", he_config.noise_floor[row][col]);
        }
    }
}

bool he_matrix_scan(matrix_row_t current_matrix[]) {
    bool updated = false;

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t sensorId = get_sensor_id_from_row_col(row, col);
            if (sensorId != 0xFF) { // Check for valid sensor ID
                select_mux(sensorId);
                uint16_t sensor_value = he_readkey_raw(sensorId);

                // Handle noise floor calibration
                //if (sensor_value < (he_config.noise_floor[row][col] - NOISE_FLOOR_THRESHOLD)) {
                  //  // Adjust the noise floor based on the current reading
                    //he_config.noise_floor[row][col] = sensor_value;
               // }

                // Update the key state based on thresholds
                updated |= he_update_key(&current_matrix[row], row, col, sensor_value);
            }
        }
    }

    return updated;
}


// Debug print key values
void he_print_matrix(void) {
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            uint8_t sensorId = get_sensor_id_from_row_col(row, col);
            if (sensorId != 0xFF) { // Valid sensor ID
                uint16_t raw_sensor_value = he_readkey_raw(sensorId); // Read the raw sensor value
                bool debounced_state = debounce_matrix[row][col].debounced_state;
                uint16_t noise_floor = he_config.noise_floor[row][col];
                uint16_t actuation_threshold = he_config.actuation_threshold;
                uint16_t release_threshold = he_config.release_threshold;

                // Add detailed debugging info
                uprintf("R%dC%d: Sensor ID=%u, Raw Value=%u, Debounced State=%s, Noise Floor=%u, Actuation Thresh=%u, Release Thresh=%u\n",
                        row, col, sensorId, raw_sensor_value,
                        debounced_state ? "PRESSED" : "RELEASED",
                        he_config.noise_floor[row][col], actuation_threshold, release_threshold);

                // Check if the current sensor value is above or below the noise floor and thresholds
                if(raw_sensor_value > noise_floor) {
                    if(raw_sensor_value >= actuation_threshold) {
                        uprintf("R%dC%d: ABOVE actuation threshold\n", row, col);
                    } else if(raw_sensor_value <= release_threshold) {
                        uprintf("R%dC%d: BELOW release threshold\n", row, col);
                    } else {
                        uprintf("R%dC%d: BETWEEN thresholds\n", row, col);
                    }
                } else {
                    uprintf("R%dC%d: BELOW noise floor\n", row, col);
                }
            } else {
                uprintf("R%dC%d: Sensor ID Invalid\n", row, col);
            }
        }
    }

}



// Rescale the value to a different range
uint16_t rescale(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
