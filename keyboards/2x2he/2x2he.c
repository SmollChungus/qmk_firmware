// 2x2he.c (keyboard.c in qmk terms)

#include "he_switch_matrix.h"

#ifdef VIA_ENABLE

eeprom_he_config_t eeprom_he_config;
via_he_config_t via_he_config;

void keyboard_post_init_kb(void) {
    // Load from EEPROM
    eeconfig_read_kb_datablock(&eeprom_he_config);

    // Check if the EEPROM values are valid
    // Assuming actuation_threshold should never be 0 in a properly initialized EEPROM
    if (eeprom_he_config.he_actuation_threshold == 0 || eeprom_he_config.he_release_threshold == 0) {
        // Set to default values
        eeprom_he_config.he_actuation_threshold = DEFAULT_ACTUATION_LEVEL;
        eeprom_he_config.he_release_threshold = DEFAULT_RELEASE_LEVEL;

        // Save the default values back to EEPROM
        eeconfig_update_kb_datablock(&eeprom_he_config);
    }

    // Apply to runtime configuration
    he_config.he_actuation_threshold = eeprom_he_config.he_actuation_threshold;
    he_config.he_release_threshold = eeprom_he_config.he_release_threshold;

    // Continue with the rest of the initialization...
        keyboard_post_init_user();
}

void via_update_config(void) {
    // Update runtime configuration
    he_config.he_actuation_threshold = via_he_config.he_actuation_threshold;
    he_config.he_release_threshold = via_he_config.he_release_threshold;
    // Optionally, update other runtime config fields...

    // Convert runtime config to EEPROM format and save
    eeprom_he_config.he_actuation_threshold = he_config.he_actuation_threshold;
    eeprom_he_config.he_release_threshold = he_config.he_release_threshold;
    eeconfig_update_kb_datablock(&eeprom_he_config);
}
#endif
