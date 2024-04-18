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

    #ifdef VIA_ENABLE


    //void apc_init_thresholds(void);
    //void apc_set_threshold(bool is_for_actuation);

    // Declaring enums for VIA config menu
    enum via_he_enums {
        // clang-format off
        id_via_he_actuation_threshold = 1,
        id_via_he_release_threshold = 2,
        id_save_threshold_data = 3,
        id_start_calibration = 4,
        id_save_calibration_data = 5,
        // clang-format on
    };



    // On Keyboard startup
    void keyboard_post_init_user(void) {
        // Read custom menu variables from memory
/*        eeconfig_read_user_datablock(&eeprom_he_key_configs);
        if (eeprom_he_key_configs[0].he_actuation_threshold == 0 && eeprom_he_key_configs[0].he_release_threshold == 0) {
            // Set to default values
            //eeprom_he_key_configs.he_actuation_mode = 0; refactor
            for (int i = 0; i < SENSOR_COUNT; i++) {
                // Add default noise floor and ceiling values here
                eeprom_he_key_configs[i].noise_floor = he_key_configs[i].noise_floor; // calibrated on startup
                eeprom_he_key_configs[i].noise_ceiling = EXPECTED_NOISE_CEILING;
                eeprom_he_key_configs[i].he_actuation_threshold = DEFAULT_ACTUATION_LEVEL;
                eeprom_he_key_configs[i].he_release_threshold = DEFAULT_RELEASE_LEVEL;

                //set via configs
                via_he_key_configs[i].he_actuation_threshold = eeprom_he_key_configs[i].he_actuation_threshold;
                via_he_key_configs[i].he_release_threshold = eeprom_he_key_configs[i].he_release_threshold;
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

                he_key_configs[i].he_actuation_threshold = eeprom_he_key_configs[i].he_actuation_threshold;
                he_key_configs[i].he_release_threshold = eeprom_he_key_configs[i].he_release_threshold;
                he_key_configs[i].noise_floor = eeprom_he_key_configs[i].noise_floor;
                he_key_configs[i].noise_ceiling = eeprom_he_key_configs[i].noise_ceiling;

                //set via configs
                via_he_key_configs[i].he_actuation_threshold = eeprom_he_key_configs[i].he_actuation_threshold;
                via_he_key_configs[i].he_release_threshold = eeprom_he_key_configs[i].he_release_threshold;

            }

            uprintf("Size of eeprom_he_key_config_t: %u bytes\n", sizeof(eeprom_he_key_configs));
    }*/
    }




    // Handle the data received by the keyboard from the VIA menus
    void via_he_config_set_value(uint8_t *data) {

        // data = [ value_id, value_data ]
        uint8_t *value_id   = &(data[0]);
        uint8_t *value_data = &(data[1]);
        uprintf("[DEBUG] Setting config value. ID: %d\n", data[0]);

        switch (*value_id) {
            case id_via_he_actuation_threshold: {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    via_he_key_configs[i].he_actuation_threshold = value_data[1] | (value_data[0] << 8);
                }
                break;
            }
            case id_via_he_release_threshold: {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    via_he_key_configs[i].he_release_threshold = value_data[1] | (value_data[0] << 8);
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
                break;
            /*case id_save_calibration_data: {
                he_config.he_calibration_mode = false; // Disable calibration mode
                print("Calibration ended, to recalibrate, hit start calibration \n");
                eeconfig_update_user_datablock(&eeprom_he_key_configs);
                break;
            }*/

        }
    }
    }
    // Handle the data sent by the keyboard to the VIA menus
    void via_he_config_get_value(uint8_t *data) {
        //data = [ value_id, value_data ]
        uint8_t *value_id   = &(data[0]);
        uint8_t *value_data = &(data[1]);
        uprintf("[DEBUG] Getting config value. ID: %d\n", data[0]);
        switch (*value_id) {
            case id_via_he_actuation_threshold: {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    value_data[0] = via_he_key_configs[i].he_actuation_threshold >> 8;
                    value_data[1] = via_he_key_configs[i].he_actuation_threshold & 0xFF;
                    uprintf("via_he_config_get_value with (%d , %d) id_via_he_actuation_threshold\n",data[0], data[1]);
                    break;
                }
            }
            case id_via_he_release_threshold: {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    value_data[0] = via_he_key_configs[i].he_release_threshold >> 8;
                    value_data[1] = via_he_key_configs[i].he_release_threshold & 0xFF;
                    uprintf("via_he_config_get_value with (%d , %d) id_via_he_release_threshold\n",data[0], data[1]);
                    break;
                    }

            }
        }
    }

    /* Save the data to persistent memory after changes are made
    void via_he_config_save(void) {
        // Debugging print
        uprintf("Saving HE config: Actuation: %u, Release: %u\n", via_he_config.he_actuation_threshold, via_he_config.he_release_threshold);
        if (eeprom_he_config.he_actuation_threshold == 0 || eeprom_he_config.he_release_threshold == 0) {
            print("don't set both to 0 plis");
        } else {
        eeconfig_update_user_datablock(&via_he_config);
        uprintf("HE config saved successfully.\n");
        }
    } */

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
                    print("id custom get value invoked, calling via_he_config_get_value");
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
                case id_start_calibration: {
                    he_config.he_calibration_mode = true; // Enable calibration mode
                    noise_ceiling_calibration();
                    noise_floor_calibration();
                    print("Calibration started, fully depress each key on the board \n");
                    break;
                }
                case id_save_calibration_data: {
                    he_config.he_calibration_mode = false; // Disable calibration mode
                    print("id_save_calibration_data invoked\n");
                    eeconfig_update_user_datablock(&eeprom_he_key_configs);

                    //via_he_calibration_save(); // Save calibration data to EEPROM
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
