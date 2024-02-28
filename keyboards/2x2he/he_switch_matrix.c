/* he_switch_matrix.c - #include "matrix.h" tasks are delegated to he_switch_matrix.*/
#include "he_switch_matrix.h"
#include "analog.h"
//#include "atomic_util.h" no need so far, needs testing
#include "print.h"
#include "wait.h"
#include "eeprom.h"

const key_to_sensor_map_t key_to_sensor_map[] = {
    {0,0,0}, {0,1,1},
    {1,0,2}, {1,1,3}
};

//id,muxId,channelId
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


static key_debounce_t debounce_matrix[MATRIX_ROWS][MATRIX_COLS] = {{{0, 0}}};

const uint32_t mux_sel_pins[] = MUX_SEL_PINS;

he_config_t he_config;

static adc_mux adcMux;

// Init function simplified for HE sensors setup
static inline void init_mux_sel(void) {
    for (int idx = 0; idx < sizeof(mux_sel_pins)/sizeof(mux_sel_pins[0]); idx++) {
        setPinOutput(mux_sel_pins[idx]);
    }
}

/* Initialize the peripherals pins */
int he_init(he_config_t const* const he_config) {
    //he_config;
    palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
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

            // Deactivate all multiplexers first - loop over aplex voor scalability? just leave multiplexers on kek?
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
    key_debounce_t *key_info = &debounce_matrix[row][col];
    bool previously_pressed = key_info->debounced_state;
    bool currently_pressed = sensor_value > he_config.he_actuation_threshold;
    bool should_release = sensor_value < he_config.he_release_threshold;

    if (currently_pressed && !previously_pressed) {
        // Key actuation logic
        if (++key_info->debounce_counter >= DEBOUNCE_THRESHOLD) {
            key_info->debounced_state = true; // Key is pressed
            current_matrix[row] |= (1UL << col);
            key_info->debounce_counter = 0;
            return true;
        }
    } else if (should_release && previously_pressed) {
        // Key release logic
        key_info->debounced_state = false; // Key is released
        current_matrix[row] &= ~(1UL << col);
        key_info->debounce_counter = 0;
        return true;
    } else {
        // Reset debounce counter if the state is stable
        key_info->debounce_counter = 0;
    }

    return false; // No change in stable state
}

bool he_matrix_scan(void) {
    bool updated = false;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t sensorId = get_sensor_id_from_row_col(row, col);
            if (sensorId != 0xFF) { // Valid sensor ID
                select_mux(sensorId);
                uint16_t sensor_value = he_readkey_raw(sensorId);
                // Assume matrix is a global matrix state accessible here
                if (he_update_key(matrix, row, col, sensor_value)) {
                    updated = true;
                }

            }
        }
    }
    return updated;
}

// Debug print key values
void he_print_matrix(void) {
    print("+---------------------------------------------------------------+\n");
    print("| Sensor Matrix                                                 |\n");
    print("+---------------------------------------------------------------+\n");
    for (int row = 0; row < MATRIX_ROWS; row++) {
        for (int col = 0; col < MATRIX_COLS; col++) {
            uint8_t sensorId = get_sensor_id_from_row_col(row, col);
            if (sensorId != 0xFF) { // Valid sensor ID
                uint16_t sensor_value = he_readkey_raw(sensorId);
                uint16_t actuation_threshold = he_config.he_actuation_threshold;
                uint16_t release_threshold = he_config.he_release_threshold;

                // Ensure the formatting aligns values neatly
                char buffer[128];
                snprintf(buffer, sizeof(buffer), "| (%d,%d): %-6u Act: %-6d Rel: %-6d |", row, col, sensor_value, actuation_threshold, release_threshold);
                print(buffer);

                if (col < (MATRIX_COLS - 1)) {
                    print(" , ");
                }
            } else {
                // Print NA for invalid sensor IDs, aligning with the above format
                char buffer[128];
                snprintf(buffer, sizeof(buffer), "| NA (%d,%d)                          |", row, col);
                print(buffer);
            }
            print("\n");
        }
        print("+---------------------------------------------------------------+\n");
    }
}
