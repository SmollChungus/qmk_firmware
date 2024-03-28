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
#include "print.h"
#include "wait.h"
#include "eeprom.h"
#include "math.h"
#include "timer.h" // Debugging
#include "raw_hid.h"




#ifdef RAW_ENABLE



void send_matrix_state_report(void) {

    uprintf("Sending matrix state report\n");

    uint8_t report[MATRIX_STATE_REPORT_SIZE] = {0};
    report[0] = MATRIX_STATE_REPORT_ID;
    for (uint8_t i = 0; i < 8; i++) {
        report[i + 1] = matrix_get_row(i) & 0xFF; // Assuming 8 rows, just an example
    }
    raw_hid_send(report, sizeof(report));
}

void send_sensor_value_report(uint8_t report_number, uint8_t start_sensor) {
    uint8_t report[SENSOR_REPORT_SIZE] = {0};
    report[0] = SENSOR_VALUE_REPORT_ID_BASE + report_number;
    for (int i = 0; i < SENSORS_PER_REPORT && (start_sensor + i) < SENSOR_COUNT; ++i) {
        // Mockup function for reading sensor value. Replace with your actual function to read sensor values.
        uint16_t sensor_value = he_readkey_raw(start_sensor + i); // Assuming this is your sensor reading function
        report[2 * i + 1] = (sensor_value >> 8) & 0xFF; // High byte
        report[2 * i + 2] = sensor_value & 0xFF; // Low byte
    }
    raw_hid_send(report, sizeof(report));
}
#endif



//row/col/sensorid/muxid/muxchannel
const sensor_to_matrix_map_t sensor_to_matrix_map[] = {
    {0,0,0,0,3},  {0,1,1,0,5},  {0,2,2,0,6},  {0,3,3,0,2},  {0,4,4,1,6},  {0,5,5,1,5},  {0,6,6,1,4},  {0,7,7,1,3},  {0,8,8,2,5},  {0,9,9,2,4},  {0,10,10,2,3},{0,11,11,2,2}, {0,12,12,3,3}, {0,13,13,3,2},{0,14,14,4,4},
    {1,0,15,0,4}, {1,1,16,0,7}, {1,2,17,0,0}, {1,3,18,0,1}, {1,4,19,1,7}, {1,5,20,1,0}, {1,6,21,1,1}, {1,7,22,1,2}, {1,8,23,2,6}, {1,9,24,2,0}, {1,10,25,2,1},{1,11,26,3,5}, {1,12,27,3,0}, {1,13,28,3,1},{1,14,29,4,2},
    {2,0,30,0,8}, {2,1,31,0,10},{2,2,32,0,15},{2,3,33,1,8}, {2,4,34,1,10},{2,5,35,1,15},{2,6,36,1,14},{2,7,37,2,8}, {2,8,38,2,9}, {2,9,39,2,15},{2,10,40,3,4},{2,11,41,3,8}, {2,12,42,3,13},{2,13,43,4,14},
    {3,0,44,0,9}, {3,1,45,0,13},{3,2,46,0,14},{3,3,47,1,11},{3,4,48,1,12},{3,5,49,2,10},{3,6,50,2,11},{3,7,51,2,12},{3,8,52,2,13},{3,9,53,2,14},{3,10,54,3,9},{3,11,55,3,11},{3,12,56,4,10},{3,13,57,4,13},
    {4,0,58,0,11},{4,1,59,0,12},{4,2,60,1,9}, {4,3,61,1,13},{4,4,62,3,10},  {4,5,63,4,9}, {4,6,64,4,11},{4,7,65,4,12}
};

matrix_row_t matrix_get_row(uint8_t row) {
    if (row < MATRIX_ROWS) {
        return matrix[row];
    } else {
        return 0;
    }
}

he_sensor_calibration_t he_sensor_calibration[SENSOR_COUNT];
static key_debounce_t debounce_matrix[MATRIX_ROWS][MATRIX_COLS] = {{{0, 0}}};
const uint32_t mux_en_pins[] = MUX_EN_PINS;
const uint32_t mux_sel_pins[] = MUX_SEL_PINS;
static adc_mux adcMux;
he_config_t he_config;

bool calibration_mode = false;

//move to he_init?
static void init_mux_sel(void) {
//    int array_size = sizeof(mux_sel_pins) / sizeof(mux_sel_pins[0]);
    for (int i = 0; i < 4; i++) {
        setPinOutput(mux_sel_pins[i]);
        writePinLow(mux_sel_pins[i]);
    }
    for (int j = 0; j < 5; j++) {
        setPinOutput(mux_en_pins[j]);
        writePinHigh(mux_en_pins[j]);
    }
}


uint16_t rescale(uint16_t sensor_value, uint8_t sensor_id) {
    uint16_t sensor_value_rescaled = 0;

    uint16_t noise_floor = he_sensor_calibration[sensor_id].noise_floor;
    uint16_t switch_ceiling = he_sensor_calibration[sensor_id].switch_ceiling;
    uint8_t target_range = 100;

    if (switch_ceiling > noise_floor) {
        // Cast to float to ensure proper division, then scale and cast back
        sensor_value_rescaled = (uint16_t)(((float)(sensor_value - noise_floor) / (switch_ceiling - noise_floor)) * target_range);
    } else {
        //printf("Invalid calibration for sensor %d\n", sensor_id);
        sensor_value_rescaled = 0;
    }

    //printf("%d \n", sensor_value_rescaled);
    return sensor_value_rescaled;
}


void noise_floor_calibration_init(void) {
    // Temporary storage for calibration samples
    print("noise_floor_calibration_init");
    uint16_t samples[NOISE_FLOOR_SAMPLE_COUNT];

    for (uint8_t sensor_id = 0; sensor_id < SENSOR_COUNT; sensor_id++) {
        uint16_t min_value = UINT16_MAX; // Initialize to the max possible value

        for (uint8_t sample = 0; sample < NOISE_FLOOR_SAMPLE_COUNT; sample++) {
            // Sample each sensor multiple times
            samples[sample] = he_readkey_raw(sensor_id);
            wait_us(5); // Wait a bit between samples to not overload the sensor
        }

        // Find the minimum value among the samples for this sensor
        for (uint8_t sample = 0; sample < NOISE_FLOOR_SAMPLE_COUNT; sample++) {
            if (samples[sample] < min_value) {
                min_value = samples[sample];
            }
        }

        // Store the minimum value as the noise floor for this sensor
        he_sensor_calibration[sensor_id].noise_floor = min_value;

        // Optional: Print the noise floor for debugging
        uprintf("Sensor %d Noise Floor: %u\n", sensor_id, min_value);
    }

    // TODO: Store the noise floor values persistently (EEPROM)
}

void noise_floor_calibration(void) {
    print("noise_floor_calibration_init");
    if (!calibration_mode) return; // Only run in calibration mode

    // Temporary storage for noise floor values
    uint16_t samples[SENSOR_COUNT][NOISE_FLOOR_SAMPLE_COUNT];
    memset(samples, 0, sizeof(samples));

    // Collect samples for each sensor
    for (uint8_t sample = 0; sample < NOISE_FLOOR_SAMPLE_COUNT; sample++) {
        for (uint8_t sensor_id = 0; sensor_id < SENSOR_COUNT; sensor_id++) {
            samples[sensor_id][sample] = he_readkey_raw(sensor_id);
            wait_us(50); // Small delay between samples
        }
    }

    // Calculate noise floor based on the 25th percentile of sorted samples for each sensor
    for (uint8_t sensor_id = 0; sensor_id < SENSOR_COUNT; sensor_id++) {
        // Sort the samples for this sensor
        qsort(samples[sensor_id], NOISE_FLOOR_SAMPLE_COUNT, sizeof(uint16_t), compare_uint16);

        // Determine the noise floor as the 25th percentile
        uint16_t noise_floor = samples[sensor_id][NOISE_FLOOR_SAMPLE_COUNT / 4];
        he_sensor_calibration[sensor_id].noise_floor = noise_floor;
    }
}

void switch_ceiling_calibration(void) {
    if (!calibration_mode) {
        print("exiting calib mode");
        return; // Exit if not in calibration mode
    }

    for (uint8_t sensor_id = 0; sensor_id < SENSOR_COUNT; sensor_id++) {
        uint16_t current_value = he_readkey_raw(sensor_id);
        //printf("id: %d, value: %d \n", sensor_id, current_value);

        // If the current value is higher than what's stored, update it
        if (current_value > he_sensor_calibration[sensor_id].switch_ceiling) {
            he_sensor_calibration[sensor_id].switch_ceiling = current_value;
            //printf("updated ceiling to %d\n \n", he_sensor_calibration[sensor_id].switch_ceiling);
        }
    }

    // This loop continuously updates the ceiling values
    // You might want to add debouncing or ensure that the key is fully pressed
}

int he_init(he_config_t const* const he_config) {
    palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
    adcMux = pinToMux(ANALOG_PORT);
    adc_read(adcMux);

    init_mux_sel();

    return 0;
}

// Sets EN and SEL pins on the multiplexer
static inline void select_mux(uint8_t sensor_id) {
    uint8_t mux_id = sensor_to_matrix_map[sensor_id].mux_id;
    uint8_t mux_channel = sensor_to_matrix_map[sensor_id].mux_channel;

    // Set all MUX enable pins high to disable them
    for (int i = 0; i < 5; i++) {
        writePinHigh(mux_en_pins[i]);
    }

    // Then set the selected MUX enable pin low to enable it
    writePinLow(mux_en_pins[mux_id]);

    // Set the MUX select pins
    for (int j = 0; j < 4; j++) {
        if (mux_channel & (1 << j)) {
            writePinHigh(mux_sel_pins[j]);
        } else {
            writePinLow(mux_sel_pins[j]);
        }
    }
    //wait_us(5); ///way too long, debug
}

// TODO scale and map to calibrated implementation
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

// Optimize scan plis
bool he_matrix_scan(void) {
    bool updated = false;

    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        uint8_t sensor_id = sensor_to_matrix_map[i].sensor_id;
        uint8_t row = sensor_to_matrix_map[i].row;
        uint8_t col = sensor_to_matrix_map[i].col;

        uint16_t sensor_value = he_readkey_raw(sensor_id);

        if (he_update_key(matrix, row, col, sensor_value)) {
            updated = true;
        }
    }
    if (calibration_mode) {
        switch_ceiling_calibration();
    }

    return updated;
}

// Debug stuff
sensor_data_t sensor_data[SENSOR_COUNT];

void add_sensor_sample(uint8_t sensor_id, uint16_t value) {
    sensor_data[sensor_id].samples[sensor_data[sensor_id].index % SAMPLE_COUNT] = value;
    sensor_data[sensor_id].index++;
}

int compare_uint16(const void *a, const void *b) {
    uint16_t val_a = *(const uint16_t*)a;
    uint16_t val_b = *(const uint16_t*)b;
    return (val_a > val_b) - (val_a < val_b);
}

double calculate_std_dev(uint8_t sensor_id) {
    double mean = 0.0;
    double std_dev = 0.0;

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        mean += sensor_data[sensor_id].samples[i];
    }
    mean /= SAMPLE_COUNT;

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        std_dev += pow(sensor_data[sensor_id].samples[i] - mean, 2);
    }
    std_dev = sqrt(std_dev / SAMPLE_COUNT);
    return std_dev;
}

double calculate_mean(uint8_t sensor_id) {
    double mean = 0.0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        mean += sensor_data[sensor_id].samples[i];
    }
    mean /= SAMPLE_COUNT;
    return mean;
}


void he_matrix_print(void) {
    print("+----------------------------------------------------------------------------+\n");
    print("| Sensor Matrix                                                              |\n");
    print("+----------------------------------------------------------------------------+\n");
    printf("calibration mode: %d \n", calibration_mode);

    for (uint8_t i = 0; i < 9; i++) {
        char buffer[512]; // Adjust buffer size if needed

        uint8_t sensor_id = sensor_to_matrix_map[i].sensor_id;
        uint8_t row = sensor_to_matrix_map[i].row;
        uint8_t col = sensor_to_matrix_map[i].col;

        uint16_t sensor_value = he_readkey_raw(sensor_id); // This reads the raw sensor value
        uint16_t noise_floor = he_sensor_calibration[sensor_id].noise_floor;
        uint16_t switch_ceiling = he_sensor_calibration[sensor_id].switch_ceiling; // Fetch switch ceiling
        uint16_t rescale_test_value = rescale(he_readkey_raw(sensor_id), sensor_id);
        // Continue to add the current sensor value to samples for statistical calculations
        add_sensor_sample(sensor_id, sensor_value);

        // Calculate mean and noise as standard deviation
        double mean = calculate_mean(sensor_id);
        double noise = calculate_std_dev(sensor_id);
        int noise_int = (int)(noise * 100); // Convert to integer representation for printing
        int mean_fixed = (int)(mean * 100); // Convert to fixed-point representation

        // Update snprintf to include the switch ceiling
        snprintf(buffer, sizeof(buffer),
                 "| Sensor %d (%d,%d): Val: %-5u Rescale: %d NF: %-5u Ceiling: %-5u Act: %-5d Rel: %-5d Mean: %d.%02d Noise: %d.%02d |\n",
                 sensor_id, row, col, sensor_value, rescale_test_value, noise_floor, switch_ceiling,
                 he_config.he_actuation_threshold, he_config.he_release_threshold,
                 mean_fixed / 100, mean_fixed % 100, noise_int / 100, abs(noise_int) % 100);

        print(buffer);
    }

    print("+----------------------------------------------------------------------------+\n");
}

