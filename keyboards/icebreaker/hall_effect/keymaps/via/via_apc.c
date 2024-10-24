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
//keyboards/icebreaker/keymaps/via/via_apc.c
#include "he_switch_matrix.h"
#include "action.h"
#include "via.h"
#include "config.h"
#include "print.h"
#include "eeprom.h"
#include "custom_rgb.h"

#ifdef VIA_ENABLE

uint16_t eeprom_save_timer = 0;
bool eeprom_save_pending = false;

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
    // clang-format on
};


    // On Keyboard startup

void keyboard_post_init_user(void) {
    calibration_warning();
}

void via_he_config_get_value(uint8_t *data);

void via_he_config_send_value(uint8_t value_id, uint16_t value) {
    uint8_t data[3];
    data[0] = value_id;
    data[1] = (uint8_t)(value >> 8); // High byte
    data[2] = (uint8_t)(value & 0xFF); // Low byte
    via_he_config_get_value(data); // todo is this needed??
}


// Handle the data received by the keyboard from the VIA menus
void via_he_config_set_value(uint8_t *data) {
    uint8_t value_id   = data[0];
    uint8_t value_data = data[1];

    uprintf("Setting config value. ID: %d, Value: %d\n", value_id, value_data);

    switch (value_id) {
        case id_via_he_actuation_threshold: {
            if (value_data >= 10 && value_data <= 90) {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    if (value_data > via_he_key_configs[i].he_release_threshold) {
                        via_he_key_configs[i].he_actuation_threshold = value_data;
                        uprintf("[SYSTEM]: Actuation threshold for sensor %d set to: %d\n", i, value_data);
                    } else {
                        uprintf("[SYSTEM]: Invalid actuation threshold value: %d for sensor %d. It must be greater than release threshold %d.\n",
                                value_data, i, via_he_key_configs[i].he_release_threshold);
                        via_he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_release_threshold + 1;
                        uprintf("[SYSTEM]: Actuation threshold for sensor %d adjusted to: %d\n",
                                i, via_he_key_configs[i].he_actuation_threshold);
                    }
                }
                eeprom_save_timer = timer_read();
                eeprom_save_pending = true;
            } else {
                uprintf("[SYSTEM]: Invalid actuation threshold value: %d. It must be between 10 and 90.\n", value_data);
            }
            break;
        }
        case id_via_he_release_threshold: {
            if (value_data >= 10 && value_data <= 90) {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    if (value_data < via_he_key_configs[i].he_actuation_threshold) {
                        via_he_key_configs[i].he_release_threshold = value_data;
                        uprintf("[SYSTEM]: Release threshold for sensor %d set to: %d\n", i, value_data);
                    } else {
                        uprintf("[SYSTEM]: Invalid release threshold value: %d for sensor %d. It must be less than actuation threshold %d.\n",
                                value_data, i, via_he_key_configs[i].he_actuation_threshold);
                        via_he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_actuation_threshold - 1;
                        uprintf("[SYSTEM]: Release threshold for sensor %d adjusted to: %d\n",
                                i, via_he_key_configs[i].he_release_threshold);
                    }
                }
                eeprom_save_timer = timer_read();
                eeprom_save_pending = true;
            } else {
                uprintf("[SYSTEM]: Invalid release threshold value: %d. It must be between 10 and 90.\n", value_data);
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
                he_key_configs[i].noise_ceiling = 570;
            }
            start_calibration_rgb(); // Add this line
            noise_ceiling_calibration();
            noise_floor_calibration();
            break;
        }

        case id_save_calibration_data: {
            he_config.he_calibration_mode = false;
            for (int i = 0; i < SENSOR_COUNT; i++) {
                eeprom_he_key_configs[i].noise_floor = he_key_configs[i].noise_floor;
                eeprom_he_key_configs[i].noise_ceiling = he_key_configs[i].noise_ceiling;
            }
            print("[SYSTEM]: Calibration ended, to recalibrate, hit start calibration inside VIA again.\n");
            eeconfig_update_user_datablock(&eeprom_he_key_configs);
            via_he_calibration_save();
            end_calibration_visual(); // Add this line
            calibration_warning();
            break;
        }
        case id_toggle_actuation_mode: {
            he_config.he_actuation_mode = value_data;
            uprintf("[SYSTEM]: Actuation mode toggled! mode: %d\n", he_config.he_actuation_mode);
            break;
        }
        case id_set_rapid_trigger_deadzone: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_rapid_trigger_configs[i].deadzone = value_data;
                eeconfig_update_user_datablock(&eeprom_he_key_configs);
            }
            uprintf("[SYSTEM]: Rapid Trigger Deadzone set to: %d\n", he_key_rapid_trigger_configs[0].deadzone);
            break;
        }
        case id_set_rapid_trigger_engage_distance: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_rapid_trigger_configs[i].engage_distance = value_data;
            }
            uprintf("[SYSTEM]: Rapid Trigger Engage Distance set to: %d\n", he_key_rapid_trigger_configs[0].engage_distance);
            break;
        }
        case id_set_rapid_trigger_disengage_distance: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_rapid_trigger_configs[i].disengage_distance = value_data;
            }
            uprintf("[SYSTEM]: Rapid Trigger Release Distance set to: %d\n", he_key_rapid_trigger_configs[0].disengage_distance);
            break;
        }
    }
}

void via_he_config_get_value(uint8_t *data) {
    uint8_t value_id   = data[0];
    uint8_t *value_data = &(data[1]);

    uprintf("[SYSTEM]: Getting config value. ID: %d\n", value_id);

    switch (value_id) {
        case id_via_he_actuation_threshold:
            *value_data = he_key_configs[0].he_actuation_threshold;
            uprintf("via_he_config_get_value with (%d) id_via_he_actuation_threshold\n", *value_data);
            break;
        case id_via_he_release_threshold:
            *value_data = he_key_configs[0].he_release_threshold;
            uprintf("via_he_config_get_value with (%d) id_via_he_release_threshold\n", *value_data);
            break;
        case id_start_calibration:
            uprintf("Button action requested - id (%d)\n", value_id);
            break;
        case id_save_threshold_data:
            // These are button actions, so we don't need to return a value
            uprintf("Button action requested - id (%d)\n", value_id);
            break;
        case id_toggle_actuation_mode:
            value_data[0] = he_config.he_actuation_mode;
            uprintf("Actuation mode requested - id, data (%d , %d)\n", value_id, value_data[0]);
            break;
        case id_set_rapid_trigger_deadzone: {
            value_data[0] = he_key_rapid_trigger_configs[0].deadzone >> 8;
            value_data[1] = he_key_rapid_trigger_configs[0].deadzone & 0xFF;
            uprintf("Deadzone requested - id, data (%d, %d %d)\n", value_id, value_data[0], value_data[1]);
            break;
        }
        case id_set_rapid_trigger_engage_distance: {
            value_data[0] = he_key_rapid_trigger_configs[0].engage_distance & 0xFF;
            uprintf("Engage distance requested - id, data (%d, %d %d)\n", value_id, value_data[0], value_data[1]);
            break;
        }
        case id_set_rapid_trigger_disengage_distance: {
            value_data[0] = he_key_rapid_trigger_configs[0].disengage_distance & 0xFF;
            uprintf("Release distance requested - id, data (%d, %d %d)\n", value_id, value_data[0], value_data[1]);
            break;
        }
        default:
            uprintf("Unhandled ID %d in via_he_config_get_value\n", value_id);
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
