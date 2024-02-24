//keyboards/he2x2/keymaps/via/via_apc.c
#include "he_switch_matrix.h"
#include "action.h"
#include "via.h"
#include "config.h"
#include "print.h"

#ifdef VIA_ENABLE


//void apc_init_thresholds(void);
//void apc_set_threshold(bool is_for_actuation);

// Declaring enums for VIA config menu
enum via_he_enums {
    // clang-format off
    id_via_he_actuation_threshold = 1,
    id_via_he_release_threshold = 2
    // clang-format on
};

/* Initializing persistent memory configuration: default values are declared and stored in PMEM
void eeconfig_init_user(void) {
    // Default values
    apc.actuation_threshold = DEFAULT_ACTUATION_LEVEL;
    apc.release_threshold   = DEFAULT_RELEASE_LEVEL;
    // Write default value to EEPROM now
    eeconfig_update_user_datablock(&apc);
    uprintf("eeconfig_init_user called\n");

} */ //already done in keyboard.c

// On Keyboard startup
void keyboard_post_init_user(void) {
    // Read custom menu variables from memory
    eeconfig_read_user_datablock(&eeprom_he_config);
    via_he_config.he_actuation_threshold = eeprom_he_config.he_actuation_threshold;
    via_he_config.he_release_threshold = eeprom_he_config.he_release_threshold;
}

// Handle the data received by the keyboard from the VIA menus
void via_he_config_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_via_he_actuation_threshold: {
            via_he_config.he_actuation_threshold = value_data[1] | (value_data[0] << 8);
            via_update_config();
            break;
        }
        case id_via_he_release_threshold: {
            via_he_config.he_release_threshold = value_data[1] | (value_data[0] << 8);
            via_update_config();
            break;
        }
    }
}

// Handle the data sent by the keyboard to the VIA menus
void via_he_config_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_via_he_actuation_threshold: {
            value_data[0] = via_he_config.he_actuation_threshold >> 8;
            value_data[1] = via_he_config.he_actuation_threshold & 0xFF;
            break;
        }
        case id_via_he_release_threshold: {
            value_data[0] = via_he_config.he_release_threshold >> 8;
            value_data[1] = via_he_config.he_release_threshold & 0xFF;
            break;
        }
    }
}

// Save the data to persistent memory after changes are made
void via_he_config_save(void) {
    eeconfig_update_user_datablock(&via_he_config);
    uprint("eeprom wear");
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id == id_custom_channel) {
        switch (*command_id) {
            case id_custom_set_value: {
                via_he_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                via_he_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                via_he_config_save();
                break;
            }
            default: {
                // Unhandled message.
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }

    *command_id = id_unhandled;
}



#endif //VIA_ENABLE2
