/* he_switch_matrix.c - #include "matrix.h" tasks are delegated to he_switch_matrix.*/
#include "he_switch_matrix.h"
#include "analog.h"
//#include "atomic_util.h" no need so far, needs testing
#include "print.h"
#include "wait.h"
#include "eeprom.h"

// map = row, col, sensor_id, mux_id, mux_channel
const sensor_to_matrix_map_t sensor_to_matrix_map[] = {
    {0,0,0,0,4},  {0,1,1,0,2},
    {1,0,2,0,12}, {1,1,3,0,13}
};

matrix_row_t matrix_get_row(uint8_t row) {
    if (row < MATRIX_ROWS) {
        return matrix[row];
    } else {
        return 0;
    }
}

static key_debounce_t debounce_matrix[MATRIX_ROWS][MATRIX_COLS] = {{{0, 0}}};

const uint8_t mux_en_pins[] = MUX_EN_PINS;
const uint32_t mux_sel_pins[] = MUX_SEL_PINS; //doesnt need to be uint32
static adc_mux adcMux;
he_config_t he_config;

static inline void init_mux_sel(void) {
    for (int idx = 0; idx < sizeof(mux_sel_pins)/sizeof(mux_sel_pins[0]); idx++) {
        setPinOutput(mux_sel_pins[idx]);
    }
}

/* Initialize the peripherals pins, gets called in matrix init */
int he_init(he_config_t const* const he_config) {
    //he_config;
    palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
    adcMux = pinToMux(ANALOG_PORT);
    adc_read(adcMux);

    init_mux_sel();
    for (int i = 0; i < sizeof(mux_en_pins); i++) {
        setPinOutput(mux_en_pins[i])
        writePinLow(mux_en_pins[i])
    }

    return 0;
}

// Selects EN and SEL pins on the multiplexer
static inline void select_mux(uint8_t sensor_id) {
    // Look up the multiplexer and channel directly from the sensor ID
    uint8_t mux_id = sensor_to_matrix_map[sensor_id].mux_id;
    uint8_t channel = sensor_to_matrix_map[sensor_id].channel;

    // Deactivate all multiplexers first
    for (int i = 0; i < sizeof(mux_en_pins); i++) {
        writePinLow(mux_en_pins[i]);
    }

    // Activate the correct multiplexer
    writePinHigh(mux_en_pins[mux_id]);

    // Set the correct channel on the activated multiplexer
    for (int j = 0; j < 4; j++) {
        if (channel & (1 << j)) {
            writePinHigh(mux_sel_pins[j]);
        } else {
            writePinLow(mux_sel_pins[j]);
        }
    }
}

// Read the HE sensor value - replace matrix with direct pin
// Function to read HE sensor value directly through MUX and ADC
uint16_t he_readkey_raw(uint8_t sensorIndex) {
    uint16_t sensor_value = 0;
    select_mux(sensorIndex);
    sensor_value = adc_read(adc_mux); // Read the ADC value for the selected sensor
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


//needs optimiziation - remove get_sensor_id_form_row_col
bool he_matrix_scan(void) {
    bool updated = false;

    // Assuming sensor_to_matrix_map is an array of sensor_to_matrix_map_t
    // and SENSOR_COUNT is the total number of sensors
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        uint8_t sensor_id = sensor_to_matrix_map[i].sensor_id;
        uint8_t row = sensor_to_matrix_map[i].row;
        uint8_t col = sensor_to_matrix_map[i].col;

        select_mux(sensor_id); // Selects the correct multiplexer channel
        uint16_t sensor_value = he_readkey_raw(sensor_id); // Read the sensor value

        // Update the matrix based on the sensor value
        if (he_update_key(matrix, row, col, sensor_value)) {
            updated = true;
        }
    }

    return updated;
}

// Debug print key values
void he_matrix_print(void) {
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
                snprintf(buffer, sizeof(buffer), "| (%d,%d): %-3u Act: %-3d Rel: %-3d |", row, col, sensor_value, actuation_threshold, release_threshold);
                print(buffer);

                if (col < (MATRIX_COLS - 1)) {
                    snprintf(row, " ROW %d");
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
