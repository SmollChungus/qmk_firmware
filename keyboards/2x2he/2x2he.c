//2x2he.c
#include "he_switch_matrix.h"

void eeconfig_init_kb(void) {
    // Default values
    //eeprom_he_config.actuation_mode                 = DEFAULT_ACTUATION_MODE;
    eeprom_he_config.actuation_threshold     = DEFAULT_ACTUATION_LEVEL;
    eeprom_he_config.release_threshold       = DEFAULT_RELEASE_LEVEL;
    //eeprom_he_config.mode_1_initial_deadzone_offset = DEFAULT_MODE_1_INITIAL_DEADZONE_OFFSET;
    //eeprom_he_config.mode_1_actuation_offset        = DEFAULT_MODE_1_ACTUATION_OFFSET;
    //eeprom_he_config.mode_1_release_offset          = DEFAULT_MODE_1_RELEASE_OFFSET;

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            eeprom_he_config.bottoming_reading[row][col] = DEFAULT_BOTTOMING_READING;
        }
    }
    // Write default value to EEPROM now
    eeconfig_update_kb_datablock(&eeprom_he_config);
}

// On Keyboard startup
void keyboard_post_init_kb(void) {
    // Read custom menu variables from memory
    eeconfig_read_kb_datablock(&eeprom_he_config);

    // Set runtime values to EEPROM values
    //he_config.actuation_mode                 = eeprom_he_config.actuation_mode;
    he_config.actuation_threshold     = eeprom_he_config.actuation_threshold;
    he_config.release_threshold       = eeprom_he_config.release_threshold;
    //he_config.mode_1_initial_deadzone_offset = eeprom_he_config.mode_1_initial_deadzone_offset;
    //he_config.mode_1_actuation_offset        = eeprom_he_config.mode_1_actuation_offset;
    //he_config.mode_1_release_offset          = eeprom_he_config.mode_1_release_offset;
    he_config.bottoming_calibration          = false;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            he_config.bottoming_calibration_starter[row][col]           = false;
            he_config.bottoming_reading[row][col]                       = eeprom_he_config.bottoming_reading[row][col];
            he_config.rescaled_actuation_threshold[row][col]     = rescale(he_config.actuation_threshold, 0, 1023, he_config.noise_floor[row][col], eeprom_he_config.bottoming_reading[row][col]);
            he_config.rescaled_release_threshold[row][col]       = rescale(he_config.release_threshold, 0, 1023, he_config.noise_floor[row][col], eeprom_he_config.bottoming_reading[row][col]);
            //he_config.rescaled_mode_1_initial_deadzone_offset[row][col] = rescale(he_config.mode_1_initial_deadzone_offset, 0, 1023, he_config.noise_floor[row][col], eeprom_he_config.bottoming_reading[row][col]);
        }
    }
}
