// add license
// he_switch_matrix.c
#include "he_switch_matrix.h"
#include "analog.h"
#include "print.h"
#include "wait.h"
#include "eeprom.h"
#include "math.h"

// map = row, col, sensor_id, mux_id, mux_channel
const sensor_to_matrix_map_t sensor_to_matrix_map[] = {
    {0,0,0,0,4},  {0,1,1,0,3},
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
const uint32_t mux_en_pins[] = MUX_EN_PINS;
const uint32_t mux_sel_pins[] = MUX_SEL_PINS;
static adc_mux adcMux;
he_config_t he_config;

static void init_mux_sel(void) {
    int array_size = sizeof(mux_sel_pins) / sizeof(mux_sel_pins[0]);
    for (int idx = 0; idx < array_size; idx++) {
        setPinOutput(mux_sel_pins[idx]);
    }
}

int he_init(he_config_t const* const he_config) {
    palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
    adcMux = pinToMux(ANALOG_PORT);
    adc_read(adcMux);

    init_mux_sel();
    for (int i = 0; i < ARRAY_SIZE(mux_en_pins); i++) {
        setPinOutput(mux_en_pins[i]);
        writePinLow(mux_en_pins[i]);
    }

    return 0;
}

// Selects EN and SEL pins on the multiplexer
static inline void select_mux(uint8_t sensor_id) {
    uint8_t mux_id = sensor_to_matrix_map[sensor_id].mux_id;
    uint8_t mux_channel = sensor_to_matrix_map[sensor_id].mux_channel;

    for (int i = 0; i < sizeof(mux_en_pins)/ sizeof(mux_en_pins[0   ]); i++) {
        writePinLow(mux_en_pins[i]);
    }

    writePinHigh(mux_en_pins[mux_id]);

    for (int j = 0; j < 4; j++) {
        if (mux_channel & (1 << j)) {
            writePinHigh(mux_sel_pins[j]);
        } else {
            writePinLow(mux_sel_pins[j]);
        }
    }
}

uint16_t he_readkey_raw(uint8_t sensorIndex) {
    uint16_t sensor_value = 0;
    select_mux(sensorIndex);
    sensor_value = adc_read(adcMux);
    return sensor_value;
}

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

    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        uint8_t sensor_id = sensor_to_matrix_map[i].sensor_id;
        uint8_t row = sensor_to_matrix_map[i].row;
        uint8_t col = sensor_to_matrix_map[i].col;

        select_mux(sensor_id);
        uint16_t sensor_value = he_readkey_raw(sensor_id);

        if (he_update_key(matrix, row, col, sensor_value)) {
            updated = true;
        }
    }

    return updated;
}

sensor_data_t sensor_data[SENSOR_COUNT];

// Function to add a sample to the sensor data ring buffer
void add_sensor_sample(uint8_t sensor_id, uint16_t value) {
    sensor_data[sensor_id].samples[sensor_data[sensor_id].index % SAMPLE_COUNT] = value;
    sensor_data[sensor_id].index++;
}

// Function to calculate the standard deviation of sensor samples
double calculate_std_dev(uint8_t sensor_id) {
    double mean = 0.0;
    double std_dev = 0.0;

    // Calculate mean
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        mean += sensor_data[sensor_id].samples[i];
    }
    mean /= SAMPLE_COUNT;

    // Calculate standard deviation
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        std_dev += pow(sensor_data[sensor_id].samples[i] - mean, 2);
    }
    std_dev = sqrt(std_dev / SAMPLE_COUNT);
    return std_dev;
}

void he_matrix_print(void) {
    print("+----------------------------------------------------------------------------+\n");
    print("| Sensor Matrix                                                              |\n");
    print("+----------------------------------------------------------------------------+\n");

    char buffer[192]; // Ensure buffer is defined and large enough for your output

    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        uint8_t sensor_id = sensor_to_matrix_map[i].sensor_id;
        uint16_t sensor_value = he_readkey_raw(sensor_id);

        // Add current sensor value to samples
        add_sensor_sample(sensor_id, sensor_value);

        // Calculate noise as standard deviation
        double noise = calculate_std_dev(sensor_id); // Ensure this function is called correctly
        int noise_int = (int)(noise * 100); // Convert to integer representation for printing

        // Format and print sensor data with noise
        snprintf(buffer, sizeof(buffer),
                 "| Sensor %d (%d,%d): Value: %-5u Act: %-5d Rel: %-5d Noise (std dev): %d.%02d |\n",
                 sensor_id, sensor_to_matrix_map[i].row, sensor_to_matrix_map[i].col,
                 sensor_value, he_config.he_actuation_threshold, he_config.he_release_threshold,
                 noise_int / 100, noise_int % 100); // Correctly format the noise value

        print(buffer);
    }

    print("+----------------------------------------------------------------------------+\n");
}
