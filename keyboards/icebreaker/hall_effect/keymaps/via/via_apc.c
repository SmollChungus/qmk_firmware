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
//Credit to Cipulot's EC boards as a foundation for HE VIA intergration
//keyboards/he2x2/keymaps/via/via_apc.c
#include "he_switch_matrix.h"
#include "action.h"
#include "via.h"
#include "config.h"
#include "print.h"
#include "eeprom.h"

#ifdef VIA_ENABLE

// Declaring enums for VIA config menu
enum via_he_enums {
    // clang-format off
    id_via_he_actuation_threshold = 1,
    id_via_he_release_threshold = 2,
    id_save_threshold_data = 3,
    id_start_calibration = 4,
    id_save_calibration_data = 5,
    id_toggle_actuation_mode = 6,
    id_set_rapid_trigger_deadzone = 7,
    id_set_rapid_trigger_engage_distance = 8,
    id_set_rapid_trigger_disengage_distance = 9,
    id_set_key_cancel = 10,
    // clang-format on
};


    // On Keyboard startup
__attribute__((weak)) void keyboard_post_init_user(void) {}


void via_he_config_get_value(uint8_t *data);

void via_he_config_send_value(uint8_t value_id, uint16_t value) {
    uint8_t data[3];
    data[0] = value_id;
    data[1] = (uint8_t)(value >> 8); // High byte
    data[2] = (uint8_t)(value & 0xFF); // Low byte
    via_he_config_get_value(data);
}


// Handle the data received by the keyboard from the VIA menus
void via_he_config_set_value(uint8_t *data) {

    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    uprintf("Setting config value. ID: %d\n", data[0]);

    switch (*value_id) {
        case id_via_he_actuation_threshold: {
            uint16_t actuation_value = value_data[1] | (value_data[0] << 8);
            for (int i = 0; i < SENSOR_COUNT; i++) {
                // Ensure actuation threshold is less than release threshold
                if (actuation_value > via_he_key_configs[i].he_release_threshold) {
                    via_he_key_configs[i].he_actuation_threshold = actuation_value;
                    uprintf("[SYSTEM]: Actuation threshold for sensor %d set to: %d\n", i, actuation_value);
                } else {
                    uprintf("[SYSTEM]: Invalid actuation threshold value: %d for sensor %d. It must be less than release threshold %d.\n",
                            actuation_value, i, via_he_key_configs[i].he_release_threshold);
                    // Adjust the actuation threshold to be slightly less than the release threshold
                    via_he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_release_threshold - 1;
                    uprintf("[SYSTEM]: Actuation threshold for sensor %d adjusted to: %d\n",
                            i, via_he_key_configs[i].he_actuation_threshold);
                    // Send the corrected value back to VIA
                    via_he_config_send_value(id_via_he_actuation_threshold, via_he_key_configs[i].he_actuation_threshold);
                }
            }
            break;
        }
        case id_via_he_release_threshold: {
            uint16_t release_value = value_data[1] | (value_data[0] << 8);
            for (int i = 0; i < SENSOR_COUNT; i++) {
                // Ensure release threshold is more than actuation threshold
                if (release_value < via_he_key_configs[i].he_actuation_threshold) {
                    via_he_key_configs[i].he_release_threshold = release_value;
                    uprintf("[SYSTEM]: Release threshold for sensor %d set to: %d\n", i, release_value);
                } else {
                    uprintf("[SYSTEM]: Invalid release threshold value: %d for sensor %d. It must be greater than actuation threshold %d.\n",
                            release_value, i, via_he_key_configs[i].he_actuation_threshold);
                    // Adjust the release threshold to be slightly more than the actuation threshold
                    via_he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_actuation_threshold - 10;
                    uprintf("[SYSTEM]: Release threshold for sensor %d adjusted to: %d\n",
                            i, via_he_key_configs[i].he_release_threshold);
                    // Send the corrected value back to VIA
                    via_he_config_send_value(id_via_he_release_threshold, via_he_key_configs[i].he_release_threshold);
                }
            }
            break;
        }
        case id_save_threshold_data: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                uprintf("saving eeprom for sensor %d from via %d to eeprom %d", i, via_he_key_configs[i].he_actuation_threshold, eeprom_he_key_configs[i].he_actuation_threshold);
                eeprom_he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_actuation_threshold;
                eeprom_he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_release_threshold;
                // and set to RT
                he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_actuation_threshold;
                he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_release_threshold;
            }
            via_he_calibration_save();
            print("[SYSTEM]: Actuation Settings Saved!\n");
            break;
        }
        case id_start_calibration: {
            print("[SYSTEM]: Calibration started, fully press each key on the board!\nBe sure to end the calibration in VIA with the button once you're done.\n");
            he_config.he_calibration_mode = true; // Enable calibration mode
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_configs[i].noise_ceiling = 550;
            }
            noise_ceiling_calibration();
            noise_floor_calibration();
            break;
        }
        case id_save_calibration_data: {
            he_config.he_calibration_mode = false; // Disable calibration mode
            for (int i = 0; i < SENSOR_COUNT; i++) {
                // Add default noise floor and ceiling values here
                eeprom_he_key_configs[i].noise_floor = he_key_configs[i].noise_floor;
                eeprom_he_key_configs[i].noise_ceiling = he_key_configs[i].noise_ceiling;
            }
            print("[SYSTEM]: Calibration ended, to recalibrate, hit start calibration inside VIA again.\n");
            eeconfig_update_user_datablock(&eeprom_he_key_configs);
            via_he_calibration_save();
            break;
        }
        case id_toggle_actuation_mode: {
            he_config.he_actuation_mode = value_data[0];
            uprintf("[SYSTEM]: Actuation mode toggled! mode: %d\n",he_config.he_actuation_mode);
            break;
        }
        case id_set_rapid_trigger_deadzone: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_rapid_trigger_configs[i].deadzone = value_data[1] | (value_data[0] << 8);
                eeconfig_update_user_datablock(&eeprom_he_key_configs);
            }

            uprintf("[SYSTEM]: Rapid Trigger Deadzone set to: %d\n",he_key_rapid_trigger_configs[0].deadzone);
            break;
        }
        case id_set_rapid_trigger_engage_distance: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_rapid_trigger_configs[i].engage_distance = value_data[0];
            }
            uprintf("[SYSTEM]: Rapid Trigger Engage Distance set to: %d\n",he_key_rapid_trigger_configs[0].engage_distance);
            break;
        }
        case id_set_rapid_trigger_disengage_distance: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_rapid_trigger_configs[i].disengage_distance = value_data[0];
            }
            uprintf("[SYSTEM]: Rapid Trigger Release Distance set to: %d\n",he_key_rapid_trigger_configs[0].disengage_distance);
            break;
        }
        case id_set_key_cancel: {
            he_config.he_keycancel = true;
            uprintf("[SYSTEM]: Key Cancelation Mode toggled: %d", he_config.he_keycancel);
        }
    }
}

void via_he_config_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    // Initialize the value_data to zero before using it to ensure clean data handling
    value_data[0] = 0;
    value_data[1] = 0;

    uprintf("[SYSTEM]: Getting config value. ID: %d, Current data: %d\n", *value_id, *value_data);

    switch (*value_id) {
        case id_via_he_actuation_threshold: {
            // Convert 16-bit threshold value into two 8-bit segments
            value_data[0] = he_key_configs[0].he_actuation_threshold >> 8; // High byte
            value_data[1] = he_key_configs[0].he_actuation_threshold & 0xFF; // Low byte
            uprintf("via_he_config_get_value with (%d , %d) id_via_he_actuation_threshold\n", value_data[0], value_data[1]);
            break;
        }
        case id_via_he_release_threshold: {
            value_data[0] = he_key_configs[0].he_release_threshold >> 8; // High byte
            value_data[1] = he_key_configs[0].he_release_threshold & 0xFF; // Low byte
            uprintf("via_he_config_get_value with (%d , %d) id_via_he_release_threshold\n", value_data[0], value_data[1]);
            break;
        }
        case id_toggle_actuation_mode: {
            value_data[0] = he_config.he_actuation_mode;
            uprintf("Actuation mode requested - id, data (%d , %d)\n", *value_id, value_data[0]);
            break;
        }
        case id_set_rapid_trigger_deadzone: {
            value_data[0] = he_key_rapid_trigger_configs[0].deadzone >> 8;
            value_data[1] = he_key_rapid_trigger_configs[0].deadzone & 0xFF;
            uprintf("Deadzone requested - id, data (%d, %d %d)\n", *value_id, value_data[0], value_data[1]);
            break;
        }
        case id_set_rapid_trigger_engage_distance: {
            value_data[0] = he_key_rapid_trigger_configs[0].engage_distance & 0xFF;
            uprintf("Release distance requested - id, data (%d, %d %d)\n", *value_id, value_data[0], value_data[1]);
            break;
        }
        case id_set_rapid_trigger_disengage_distance: {
            value_data[0] = he_key_rapid_trigger_configs[0].disengage_distance & 0xFF;
            uprintf("Release distance requested - id, data (%d, %d %d)\n", *value_id, value_data[0], value_data[1]);
            break;
        }
        default:
            uprintf("Unhandled ID %d in via_he_config_get_value\n", *value_id);
    }
}

void via_he_calibration_save(void) {
    eeconfig_update_user_datablock(&eeprom_he_key_configs);
    uprintf("Size of eeprom_he_key_config_t: %u bytes\n", sizeof(eeprom_he_key_configs));
    uprint("EEPROM wear!!!\n");
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    // data = [ command_id, channel_id, value_id, value_data ]
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id == id_custom_channel) {
        switch (*command_id) {
            case id_custom_set_value: {
                print("id custom set value invoked, calling via_he_config_set_value\n");
                via_he_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                print("id custom get value invoked, calling via_he_config_get_value\n");
                via_he_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                // Bypass
                break;
            }
            case id_save_threshold_data: {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    uprintf("saving eeprom for sensor %d from via %d to eeprom %d", i, via_he_key_configs[i].he_actuation_threshold, eeprom_he_key_configs[i].he_actuation_threshold);
                    eeprom_he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_actuation_threshold;
                    eeprom_he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_release_threshold;
                }
                //via_he_calibration_save();
                break;
            }
            default: {
                // Unhandled message.
                *command_id = id_unhandled;
                uprintf("unhandled ID, d0: %d d1 %d\n", data[0], data[1]);
                break;
            }
        }
        return;
    }

    *command_id = id_unhandled;
}
#endif