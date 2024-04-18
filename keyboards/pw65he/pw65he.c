// 2x2he.c (keyboard.c in qmk terms)

#include "he_switch_matrix.h"
#include "print.h" //debug

#ifdef VIA_ENABLE


void keyboard_post_init_kb(void) {
    keyboard_post_init_user();
    /* Assuming actuation_threshold should never be 0 in a properly initialized EEPROM
    if (eeprom_he_key_configs[0].he_actuation_threshold == 0 && eeprom_he_key_configs[0].he_release_threshold == 0) {
        // Set to default values
        //eeprom_he_key_configs.he_actuation_mode = 0; refactor
        for (int i = 0; i < SENSOR_COUNT; i++) {
            // Add default noise floor and ceiling values here
            eeprom_he_key_configs[i].noise_floor = EXPECTED_NOISE_FLOOR;
            eeprom_he_key_configs[i].noise_ceiling = EXPECTED_NOISE_CEILING;
        }


        // Save the default values back to EEPROM
        eeconfig_update_user_datablock(&eeprom_he_key_configs);
    } else {
    // Apply to runtime configuration
    for (int i = 0; i < SENSOR_COUNT; i++) {
            he_key_configs[i].he_actuation_threshold = eeprom_he_key_configs[i].he_actuation_threshold;
            he_key_configs[i].he_release_threshold = eeprom_he_key_configs[i].he_release_threshold;
            // Add default noise floor and ceiling values here
            he_key_configs[i].noise_floor = eeprom_he_key_configs[i].noise_floor;
            he_key_configs[i].noise_ceiling = eeprom_he_key_configs[i].noise_ceiling;

        }
    }
} */
}

void via_update_config(void) {
    // Update runtime configuration
    if (via_he_key_configs[0].he_actuation_threshold == 0 && via_he_key_configs[0].he_release_threshold == 0) {
        print("dont set to 0 please \n");
    } else {
        he_config.he_actuation_mode = via_he_config.he_actuation_mode;
        he_config.he_calibration_mode = via_he_config.he_calibration_mode;
        for (int i = 0; i < SENSOR_COUNT; i++) {
            he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_actuation_threshold;
            he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_release_threshold;
            //he_key_configs[i].noise_floor = via_he_key_configs[i].noise_floor;
            //he_key_configs[i].noise_ceiling = via_he_key_configs[i].noise_ceiling;
            // Convert runtime config to EEPROM format and save
            eeprom_he_key_configs[i].he_actuation_threshold = he_key_configs[i].he_actuation_threshold;
            eeprom_he_key_configs[i].he_release_threshold = he_key_configs[i].he_release_threshold;
            // delete -> eeprom_he_key_configs[i].he_actuation_mode = he_key_configs[i].he_actuation_mode;
    eeconfig_update_user_datablock(&eeprom_he_key_configs);
    print("saved actuation thresholds to USER EEPROM \n");
        }
    }
}
#endif
