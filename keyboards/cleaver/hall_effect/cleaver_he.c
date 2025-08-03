
#include "he_switch_matrix.h"
#include "print.h" //debug

#ifdef VIA_ENABLE


void keyboard_post_init_kb(void) {
    keyboard_post_init_user();
}


// is this ever called actually????? // no but triplwxhwxk
void via_update_config(void) {
    // Update runtime configuration
    if (via_he_key_configs[0].he_actuation_threshold == 0 && via_he_key_configs[0].he_release_threshold == 0) {
        print("dont set to 0 please owo -- is this ever called?? \n"); //fix this
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
        }
    eeconfig_update_user_datablock(&eeprom_he_key_configs);
    print("saved actuation thresholds to USER EEPROM---- is this ever called??\n");
    }
}
#endif
