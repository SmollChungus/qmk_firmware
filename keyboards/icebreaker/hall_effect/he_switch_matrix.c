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
#include "custom_rgb.h"


#ifdef VIA_ENABLE
eeprom_he_config_t eeprom_he_config;
via_he_config_t via_he_config;

eeprom_he_key_config_t eeprom_he_key_configs[SENSOR_COUNT];
via_he_key_config_t via_he_key_configs[SENSOR_COUNT];
#endif

//data = row,col,sensor_id,mux_id,mux_channel
const sensor_to_matrix_map_t sensor_to_matrix_map[] = {
    {0,0,0,0,3},  {0,1,1,0,5},  {0,2,2,0,6},  {0,3,3,0,2},  {0,4,4,1,6},  {0,5,5,1,5},  {0,6,6,1,4},  {0,7,7,1,3},  {0,8,8,2,5},  {0,9,9,2,4},  {0,10,10,2,3},{0,11,11,2,2}, {0,12,12,3,3}, {0,13,13,3,2},{0,14,14,4,5},{0,15,15,4,4},
    {1,0,16,0,4}, {1,1,17,0,7}, {1,2,18,0,0}, {1,3,19,0,1}, {1,4,20,1,7}, {1,5,21,1,0}, {1,6,22,1,1}, {1,7,23,1,2}, {1,8,24,2,6}, {1,9,25,2,0}, {1,10,26,2,1},{1,11,27,3,5}, {1,12,28,3,0}, {1,13,29,3,1},{1,14,30,4,2},
    {2,0,31,0,8}, {2,1,32,0,10},{2,2,33,0,15},{2,3,34,1,8}, {2,4,35,1,10},{2,5,36,1,15},{2,6,37,1,14},{2,7,38,2,8}, {2,8,39,2,9}, {2,9,40,2,15},{2,10,41,3,8},{2,11,42,3,9}, {2,12,43,3,14},{2,13,44,4,14},
    {3,0,45,0,9}, {3,1,46,0,13},{3,2,47,0,14},{3,3,48,1,11},{3,4,49,1,12},{3,5,50,2,10},{3,6,51,2,11},{3,7,52,2,12},{3,8,53,2,13},{3,9,54,2,14},{3,10,55,3,10},{3,11,56,3,13},{3,12,57,4,10},{3,13,58,4,13},
    {4,0,59,0,11},{4,1,60,0,12},{4,2,61,1,9}, {4,3,62,1,13},{4,4,63,3,11},{4,5,64,3,12},{4,6,65,4,9}, {4,7,66,4,11},{4,8,67,4,12}
};


// key cancellation stuff, implement xelus's pr(24k) whenever it hits master
uint8_t latest_pressed = 0;
bool cancel_lock = false;

matrix_row_t matrix_get_row(uint8_t row) {
    if (row < MATRIX_ROWS) {
        return matrix[row];
    } else {
        return 0;
    }
}

// delete => he_sensor_calibration_t he_sensor_calibration[SENSOR_COUNT];
static key_debounce_t debounce_matrix[MATRIX_ROWS][MATRIX_COLS] = {{{0, 0}}};
const uint32_t mux_en_pins[] = MUX_EN_PINS;
const uint32_t mux_sel_pins[] = MUX_SEL_PINS;
static adc_mux adcMux;
he_config_t he_config;
he_key_config_t he_key_configs[SENSOR_COUNT];
he_key_rapid_trigger_config_t he_key_rapid_trigger_configs[SENSOR_COUNT];


static void mux_sel_init(void) {
    for (int i = 0; i < 4; i++) {
        setPinOutput(mux_sel_pins[i]);
        writePinLow(mux_sel_pins[i]);
    }
    for (int j = 0; j < 5; j++) {
        setPinOutput(mux_en_pins[j]);
        writePinHigh(mux_en_pins[j]);
    }
}


uint8_t rescale(uint16_t sensor_value, uint8_t sensor_id) {
    uint16_t noise_floor = he_key_configs[sensor_id].noise_floor; //raw read
    uint16_t noise_ceiling = he_key_configs[sensor_id].noise_ceiling; //raw read

    if (noise_ceiling > noise_floor) {
        if (sensor_value <= noise_floor) return 0;
        if (sensor_value >= noise_ceiling) return 100;
        return (uint8_t)(((uint32_t)(sensor_value - noise_floor) * 100) / (noise_ceiling - noise_floor));
    }
    return 0;
}


void noise_floor_calibration_init(void) {
    // Temporary storage for calibration samples
    print("noise_floor_calibration_init"); //  i think this happens during boot before hid console connects so yeah
    uint16_t samples[NOISE_FLOOR_SAMPLE_COUNT];
    eeconfig_read_user_datablock(&eeprom_he_key_configs);
    for (uint8_t sensor_id = 0; sensor_id < SENSOR_COUNT; sensor_id++) {
        uint16_t min_value = UINT16_MAX; // Initialize to the max possible value

        for (uint8_t sample = 0; sample < NOISE_FLOOR_SAMPLE_COUNT; sample++) {
            samples[sample] = he_readkey_raw(sensor_id);
            if (samples[sample] < min_value) {
                min_value = samples[sample];
            }
            wait_us(5);
        }
        he_key_configs[sensor_id].noise_floor = min_value;
        eeprom_he_key_configs[sensor_id].noise_floor = min_value;

        // Optional: Print the noise floor for debugging
        uprintf("Sensor %d Noise Floor: %u\n", sensor_id, min_value);
    }
    eeconfig_update_user_datablock(&eeprom_he_key_configs);
}

void noise_floor_calibration(void) {
    print("noise_floor_calibration");
    if (!he_config.he_calibration_mode) return;

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
        he_key_configs[sensor_id].noise_floor = noise_floor;
        eeprom_he_key_configs[sensor_id].noise_floor = noise_floor;

    }
}

void noise_ceiling_calibration(void) {
    if (!he_config.he_calibration_mode) {
        print("exiting calibration mode");
        return;
    }
    const uint8_t NUM_READINGS = 5;  // todo move to config.h
    bool led_trigger = false;

    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        uint16_t range = he_key_configs[i].noise_ceiling - he_key_configs[i].noise_floor;
        uint16_t ceiling_range = he_key_configs[i].noise_ceiling - (range * 0.02);
        uint16_t sum = 0;
        uint16_t average_value = 0;

        // Take multiple readings and calculate their average
        for (uint8_t j = 0; j < NUM_READINGS; j++) {
            sum += he_readkey_raw(i);
        }
        average_value = sum / NUM_READINGS;

        // Update ceiling if the average reading is above the ceiling range
        if (average_value > ceiling_range) {
            he_key_configs[i].noise_ceiling = average_value;
            update_calibration_rgb(i, average_value);
            led_trigger  = true;
            printf("Updated ceiling for sensor %d to %d\n", i, average_value);
        }
    }

    if (led_trigger) {
        apply_calibration_changes_rgb();
    }
}

int he_init(he_key_config_t he_key_configs[], size_t count) {
    palSetLineMode(ANALOG_PORT, PAL_MODE_INPUT_ANALOG);
    adcMux = pinToMux(ANALOG_PORT);
    adc_read(adcMux);

    mux_sel_init();
    // check and reset for non-sense user values
    #ifdef VIA_ENABLE
    eeconfig_read_kb_datablock(&eeprom_he_config);
    eeconfig_read_user_datablock(&eeprom_he_key_configs);

    if (eeprom_he_config.he_post_flash == false) {
        eeprom_he_config.he_post_flash = true;
        // set RT and eeprom from defaults
        for (int i = 0; i < SENSOR_COUNT; i++) {
            he_key_configs[i].he_actuation_threshold = DEFAULT_ACTUATION_LEVEL;
            he_key_configs[i].he_release_threshold = DEFAULT_RELEASE_LEVEL;
            he_key_configs[i].noise_ceiling = EXPECTED_NOISE_CEILING;
            eeprom_he_key_configs[i].he_actuation_threshold = DEFAULT_ACTUATION_LEVEL;
            eeprom_he_key_configs[i].he_release_threshold = DEFAULT_RELEASE_LEVEL;
            eeprom_he_key_configs[i].noise_ceiling = EXPECTED_NOISE_CEILING;
            via_he_key_configs[i].he_actuation_threshold = DEFAULT_ACTUATION_LEVEL;
            via_he_key_configs[i].he_release_threshold = DEFAULT_RELEASE_LEVEL;
            he_key_rapid_trigger_configs[i].engage_distance = DEFAULT_RELEASE_DISTANCE_RT;
            he_key_rapid_trigger_configs[i].disengage_distance = DEFAULT_RELEASE_DISTANCE_RT;
            he_key_rapid_trigger_configs[i].deadzone = DEFAULT_DEADZONE_RT;
            he_key_rapid_trigger_configs[i].boundary_value = DEFAULT_DEADZONE_RT;
        }
    } else {
        for (int i = 0; i < SENSOR_COUNT; i++) {
        he_key_configs[i].he_actuation_threshold = eeprom_he_key_configs[i].he_actuation_threshold;
        he_key_configs[i].he_release_threshold = eeprom_he_key_configs[i].he_release_threshold;
        via_he_key_configs[i].he_actuation_threshold = eeprom_he_key_configs[i].he_actuation_threshold;
        via_he_key_configs[i].he_release_threshold = eeprom_he_key_configs[i].he_release_threshold;
        he_key_configs[i].noise_ceiling = eeprom_he_key_configs[i].noise_ceiling;
        he_key_rapid_trigger_configs[i].deadzone = DEFAULT_DEADZONE_RT;
        he_key_rapid_trigger_configs[i].engage_distance = DEFAULT_RELEASE_DISTANCE_RT;
        he_key_rapid_trigger_configs[i].disengage_distance = DEFAULT_RELEASE_DISTANCE_RT;
        }
    }
    eeconfig_update_user_datablock(&eeprom_he_key_configs);
    eeconfig_update_kb_datablock(&eeprom_he_config);

    //init RT stuff
    #endif
    return 0;
}

// Sets EN and SEL pins on the multiplexer during scanning
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
}

// TODO scale and map to calibrated implementation
uint16_t he_readkey_raw(uint8_t sensorIndex) {
    select_mux(sensorIndex);
    return adc_read(adcMux);
}

bool he_update_key(matrix_row_t* current_matrix, uint8_t row, uint8_t col, uint8_t sensor_id, uint16_t sensor_value) {
    key_debounce_t *key_info = &debounce_matrix[row][col];
    bool previously_pressed = key_info->debounced_state;
    uint8_t rescaled_value = rescale(sensor_value, sensor_id);
    bool currently_pressed = rescaled_value > he_key_configs[sensor_id].he_actuation_threshold;
    bool should_release = rescaled_value < he_key_configs[sensor_id].he_release_threshold;

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

bool he_update_key_rapid_trigger(matrix_row_t* current_matrix, uint8_t row, uint8_t col, uint8_t sensor_id, uint16_t sensor_value) {
    key_debounce_t *key_info = &debounce_matrix[row][col];
    uint8_t deadzone = he_key_rapid_trigger_configs[sensor_id].deadzone;
    uint8_t disengage_distance = he_key_rapid_trigger_configs[sensor_id].disengage_distance;
    uint8_t engage_distance = he_key_rapid_trigger_configs[sensor_id].engage_distance;
    uint8_t hysteresis_margin = 8;
    uint8_t* boundary_value = &he_key_rapid_trigger_configs[sensor_id].boundary_value;

    uint8_t rescaled_value = rescale(sensor_value, sensor_id);

    if (rescaled_value <= (deadzone - hysteresis_margin)) {
        key_info->debounced_state = false;
        *boundary_value = deadzone;
        current_matrix[row] &= ~(1UL << col);
        key_info->debounce_counter = 0;
        return true;
    }

    // Only process if we're above deadzone + hysteresis
    if (rescaled_value > (deadzone + hysteresis_margin)) {
        bool currently_pressed = rescaled_value > (*boundary_value + engage_distance);
        bool should_release = rescaled_value < (*boundary_value - disengage_distance);

        if (currently_pressed) {
            if (!key_info->debounced_state) {
                if (++key_info->debounce_counter >= DEBOUNCE_THRESHOLD) {
                    key_info->debounced_state = true;
                    *boundary_value = rescaled_value;
                    current_matrix[row] |= (1UL << col);
                    key_info->debounce_counter = 0;
                    return true;
                }
            } else {
                // Update boundary only if we detect significant movement upward
                if (rescaled_value > (*boundary_value + hysteresis_margin)) {
                    *boundary_value = rescaled_value;
                }
            }
        } else if (should_release && key_info->debounced_state) {
            if (++key_info->debounce_counter >= DEBOUNCE_THRESHOLD) {
                key_info->debounced_state = false;
                *boundary_value = rescaled_value + engage_distance;
                current_matrix[row] &= ~(1UL << col);
                key_info->debounce_counter = 0;
                return true;
            }
        } else {
            key_info->debounce_counter = 0;
        }
    }
    return false;
}

bool he_update_key_keycancel(matrix_row_t* current_matrix, uint8_t row, uint8_t col, uint8_t sensor_id, uint16_t sensor_value) {
    key_debounce_t *key_info = &debounce_matrix[row][col];
    bool previously_pressed = key_info->debounced_state;
    uint8_t rescaled_value = rescale(sensor_value, sensor_id);
    bool currently_pressed = rescaled_value > he_key_configs[sensor_id].he_actuation_threshold;
    bool should_release = rescaled_value < he_key_configs[sensor_id].he_release_threshold;
    static bool a_physically_pressed = false;
    static bool d_physically_pressed = false;

    // Handle key press
    if (currently_pressed && !previously_pressed) {
        if (++key_info->debounce_counter >= DEBOUNCE_THRESHOLD) {
            // Update physical state tracking
            if (sensor_id == 32) a_physically_pressed = true;
            if (sensor_id == 34) d_physically_pressed = true;

            // Special handling for A and D keys (sensor 32 and 34)
            if ((sensor_id == 34 && latest_pressed == 32) ||
                (sensor_id == 32 && latest_pressed == 34)) {
                // Cancel the previous key
                uint8_t prev_sensor = (sensor_id == 34) ? 32 : 34;
                current_matrix[sensor_to_matrix_map[prev_sensor].row] &= ~(1UL << sensor_to_matrix_map[prev_sensor].col);
            }

            // Press the current key
            key_info->debounced_state = true;
            current_matrix[row] |= (1UL << col);
            key_info->debounce_counter = 0;
            latest_pressed = sensor_id;
            return true;
        }
    }
    // Handle key release
    else if (should_release && previously_pressed) {
        // Update physical state tracking
        if (sensor_id == 32) a_physically_pressed = false;
        if (sensor_id == 34) d_physically_pressed = false;

        key_info->debounced_state = false;
        current_matrix[row] &= ~(1UL << col);
        key_info->debounce_counter = 0;

        // If releasing the currently active key, check if we should restore the other key
        if (latest_pressed == sensor_id) {
            if (sensor_id == 32 && d_physically_pressed) {
                // If releasing A and D is still physically pressed, restore D
                current_matrix[sensor_to_matrix_map[34].row] |= (1UL << sensor_to_matrix_map[34].col);
                latest_pressed = 34;
            } else if (sensor_id == 34 && a_physically_pressed) {
                // If releasing D and A is still physically pressed, restore A
                current_matrix[sensor_to_matrix_map[32].row] |= (1UL << sensor_to_matrix_map[32].col);
                latest_pressed = 32;
            } else {
                latest_pressed = 0;
            }
        }
        return true;
    }
    else {
        key_info->debounce_counter = 0;
    }

    return false;
}



bool he_matrix_scan(void) {
    bool updated = false;

    if (he_config.he_calibration_mode) {
        noise_ceiling_calibration();
        return false;
    }

    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        uint8_t sensor_id = sensor_to_matrix_map[i].sensor_id;
        uint8_t row = sensor_to_matrix_map[i].row;
        uint8_t col = sensor_to_matrix_map[i].col;
        uint16_t sensor_value = he_readkey_raw(sensor_id);

        if (he_config.he_actuation_mode == 0) {
            if (he_update_key(matrix, row, col, sensor_id, sensor_value)) {
                   updated = true;
            }
        }
        else if (he_config.he_actuation_mode == 1) {
            if (he_update_key_rapid_trigger(matrix, row, col, sensor_id, sensor_value)) {
            updated = true;
            }
        }
        else if (he_config.he_actuation_mode == 2) {
            if (he_update_key_keycancel(matrix, row, col, sensor_id, sensor_value)) {
                updated = true;
            }
        }
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
#ifdef CONSOLE_ENABLE

void he_matrix_print(void) {
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        char buffer[512]; // Adjust buffer size if needed

        uint8_t sensor_id = sensor_to_matrix_map[i].sensor_id;
        uint8_t rescale_test_value = rescale(he_readkey_raw(sensor_id), sensor_id);
        uint8_t row = sensor_to_matrix_map[i].row;
        uint8_t col = sensor_to_matrix_map[i].col;

        // Update snprintf to include the switch ceiling
        snprintf(buffer, sizeof(buffer),
                 "| (%d,%d) Rescale: %d |\n",
                 row,col,rescale_test_value);

        print(buffer);
    }
}

void he_matrix_print_extended(void) {
    print("+----------------------------------------------------------------------------+\n");
    print("| Sensor Matrix                                                              |\n");
    print("+----------------------------------------------------------------------------+\n");
    printf("calibration mode: %d \n", he_config.he_calibration_mode);
    printf("post flash: %d \n", eeprom_he_config.he_post_flash);
    printf("RaTr Mode: %d \n", he_config.he_actuation_mode);
    uint16_t eesize = sizeof(eeprom_he_config) + sizeof(eeprom_he_key_configs);
    printf("eesize: %d \n", eesize);
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        char buffer[512];
        uint8_t row = sensor_to_matrix_map[i].row;
        uint8_t col = sensor_to_matrix_map[i].col;

        uint16_t sensor_value = he_readkey_raw(i);
        uint16_t noise_floor = he_key_configs[i].noise_floor;
        uint16_t noise_ceiling = he_key_configs[i].noise_ceiling;
        uint8_t rescale_test_value = rescale(he_readkey_raw(i), i);
        add_sensor_sample(i, sensor_value);

        double mean = calculate_mean(i);
        double noise = calculate_std_dev(i);
        int noise_int = (int)(noise * 100);
        int mean_fixed = (int)(mean * 100);

        if (he_config.he_actuation_mode == 0) {
            snprintf(buffer, sizeof(buffer),
                    "| Sensor %d (%d,%d): Val: %-5u Rescale: %d NF: %-5u (ee: %-5u) Ceiling: %-5u (ee: %-5u) Act: %-5d Rel: %-5d Mean: %d.%02d Noise: %d.%02d |\n",
                    i,
                    row,
                    col,
                    sensor_value,
                    rescale_test_value,
                    noise_floor,
                    eeprom_he_key_configs[i].noise_floor,
                    noise_ceiling,
                    eeprom_he_key_configs[i].noise_ceiling,
                    he_key_configs[i].he_actuation_threshold,
                    he_key_configs[i].he_release_threshold,
                    mean_fixed / 100,
                    mean_fixed % 100,
                    noise_int / 100,
                    abs(noise_int) % 100);
            print(buffer);
        }
        if (he_config.he_actuation_mode == 1) {
            snprintf(buffer, sizeof(buffer),
                 "| Sensor %d (%d,%d): Val: %-5u Rescale: %d NF: %-5u (ee: %-5u) Ceiling: %-5u (ee: %-5u) Deadzone: %-5d Engage Dt: %-5d Disengage Dt: %-5d Boundary value: %-5d  Mean: %d.%02d Noise: %d.%02d |\n",
                i,
                row,
                col,
                sensor_value,
                rescale_test_value,
                noise_floor,
                eeprom_he_key_configs[i].noise_floor,
                noise_ceiling,
                eeprom_he_key_configs[i].noise_ceiling,
                he_key_rapid_trigger_configs[i].deadzone,
                he_key_rapid_trigger_configs[i].engage_distance,
                he_key_rapid_trigger_configs[i].disengage_distance,
                he_key_rapid_trigger_configs[i].boundary_value,
                mean_fixed / 100,
                mean_fixed % 100,
                noise_int / 100,
                abs(noise_int) % 100);
            print(buffer);
        }
    }
}

void he_matrix_print_rapid_trigger(void) {
    print("+----------------------------------------------------------------------------+\n");
    print("| Sensor Matrix  RAPID TRIGGER                                               |\n");
    print("+----------------------------------------------------------------------------+\n");
    printf("calibration mode: %d \n", he_config.he_calibration_mode);
    printf("post flash: %d \n", eeprom_he_config.he_post_flash);
    printf("RaTr Mode: %d \n", he_config.he_actuation_mode);
    uint16_t eesize = sizeof(eeprom_he_config) + sizeof(eeprom_he_key_configs);
    printf("eesize: %d \n", eesize);
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        char buffer[512]; // Adjust buffer size if needed

        uint8_t row = sensor_to_matrix_map[i].row;
        uint8_t col = sensor_to_matrix_map[i].col;

        uint16_t sensor_value = he_readkey_raw(i); // This reads the raw sensor value
        uint16_t noise_floor = he_key_configs[i].noise_floor;
        uint16_t noise_ceiling = he_key_configs[i].noise_ceiling; // Fetch switch ceiling
        uint8_t rescale_test_value = rescale(he_readkey_raw(i), i);
        // Continue to add the current sensor value to samples for statistical calculations
        add_sensor_sample(i, sensor_value);

        // Calculate mean and noise as standard deviation
        double mean = calculate_mean(i);
        double noise = calculate_std_dev(i);
        int noise_int = (int)(noise * 100); // Convert to integer representation for printing
        int mean_fixed = (int)(mean * 100); // Convert to fixed-point representation
        snprintf(buffer, sizeof(buffer),
                 "| Sensor %d (%d,%d): Val: %-5u Rescale: %d NF: %-5u (ee: %-5u) Ceiling: %-5u (ee: %-5u) Deadzone: %-5d Engage Dt: %-5d Disengage Dt: %-5d Boundary value: %-5d  Mean: %d.%02d Noise: %d.%02d |\n",
                i,
                row,
                col,
                sensor_value,
                rescale_test_value,
                noise_floor,
                eeprom_he_key_configs[i].noise_floor,
                noise_ceiling,
                eeprom_he_key_configs[i].noise_ceiling,
                he_key_rapid_trigger_configs[i].deadzone,
                he_key_rapid_trigger_configs[i].engage_distance,
                he_key_rapid_trigger_configs[i].disengage_distance,
                he_key_rapid_trigger_configs[i].boundary_value,
                mean_fixed / 100,
                mean_fixed % 100,
                noise_int / 100,
                abs(noise_int) % 100);

        print(buffer);

        }
    }


void he_matrix_print_rapid_trigger_debug(void) {
    uint8_t i = 15;
    uint8_t row = sensor_to_matrix_map[i].row;
    uint8_t col = sensor_to_matrix_map[i].col;


    key_debounce_t *key_info = &debounce_matrix[row][col];
    uint16_t sensor_value = he_readkey_raw(i); // This reads the raw sensor value
    uint8_t rescale_test_value = rescale(he_readkey_raw(i), i);


    uprintf("i: %d, sensor: %d, rescale: %d, deadzone: %d, engage: %d, disengage: %d, boundary: %d, debounced_state: %d, debounce_counter: %d\n",
            i,
            sensor_value,
            rescale_test_value,
            he_key_rapid_trigger_configs[i].deadzone,
            he_key_rapid_trigger_configs[i].engage_distance,
            he_key_rapid_trigger_configs[i].disengage_distance,
            he_key_rapid_trigger_configs[i].boundary_value,
            key_info->debounced_state,
            key_info->debounce_counter);
}


#endif

