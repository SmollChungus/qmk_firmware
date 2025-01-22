/* Copyright 2025 Matthijs Muller
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

#include "hall_effect.h"
#include "action.h"
#include "via.h"
#include "config.h"
#include "print.h"
#include "eeprom.h"
#include "icebreaker_rgb.h"

uint16_t    eeprom_save_timer = 0;
bool        eeprom_save_pending = false;
extern void start_slider_visualization(uint8_t value);

enum via_he_enums {
    // clang-format off
    id_via_he_actuation_threshold = 1,
    id_via_he_release_threshold = 2,
    id_save_threshold_data = 3, // todo remove
    id_start_calibration = 4,
    id_save_calibration_data = 5,
    id_toggle_actuation_mode = 6,
    id_set_rapid_trigger_deadzone = 7,
    id_set_rapid_trigger_engage_distance = 8,
    id_set_rapid_trigger_disengage_distance = 9,
    // clang-format on
};

void via_he_config_send_value(uint8_t value_id, uint16_t value) {
    uint8_t data[3];
    data[0] = value_id;
    data[1] = (uint8_t)(value >> 8);
    data[2] = (uint8_t)(value & 0xFF); 
    via_he_config_get_value(data); 
}

void via_he_config_set_value(uint8_t *data) {
    uint8_t value_id   = data[0];
    uint8_t value_data = data[1];

    switch (value_id) {
        case id_via_he_actuation_threshold: {
            if (value_data >= 10 && value_data <= 90) {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    if (value_data > via_he_key_configs[i].he_release_threshold) {
                        via_he_key_configs[i].he_actuation_threshold = value_data;
                    } else {
                        via_he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_release_threshold + 1;
                    }
                }
                last_moved_slider = SLIDER_TYPE_ACTUATION;
                current_slider_type = SLIDER_TYPE_ACTUATION;
                slider_active = true;
                start_slider_visualization(value_data);
                eeprom_save_timer = timer_read();
                eeprom_save_pending = true;
            } else {
                /* Invalid threshold, do nothing */
            }
            break;
        }
        case id_via_he_release_threshold: {
            if (value_data >= 10 && value_data <= 90) {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    if (value_data < via_he_key_configs[i].he_actuation_threshold) {
                        via_he_key_configs[i].he_release_threshold = value_data;
                    } else {
                        via_he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_actuation_threshold - 1;
                    }
                }
                last_moved_slider = SLIDER_TYPE_RELEASE;
                current_slider_type = SLIDER_TYPE_RELEASE;
                slider_active = true;
                start_slider_visualization(value_data);
                eeprom_save_timer = timer_read();
                eeprom_save_pending = true;
            } else {
                /* Invalid threshold, do nothing */
            }
            break;
        }

        case id_start_calibration: {
            he_config.he_calibration_mode = true;
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_configs[i].noise_ceiling = 570;
            }
            start_calibration_rgb(); 
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
            eeconfig_update_user_datablock(&eeprom_he_key_configs);
            via_he_calibration_save();
            end_calibration_visual();
            calibration_warning();
            break;
        }

        case id_toggle_actuation_mode: {
            he_config.he_actuation_mode = value_data;
            break;
        }

        case id_set_rapid_trigger_deadzone: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_rapid_trigger_configs[i].deadzone = value_data;
                eeconfig_update_user_datablock(&eeprom_he_key_configs);
            }
            last_moved_slider = SLIDER_TYPE_RTP_DEADZONE;
            current_slider_type = SLIDER_TYPE_RTP_DEADZONE;
            slider_active = true;
            start_slider_visualization(value_data);
            eeprom_save_timer = timer_read();
            eeprom_save_pending = true;
            break;
        }
        case id_set_rapid_trigger_engage_distance: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_rapid_trigger_configs[i].engage_distance = value_data;
            }
            last_moved_slider = SLIDER_TYPE_RTP_ENGAGE;
            current_slider_type = SLIDER_TYPE_RTP_ENGAGE;
            slider_active = true;
            start_slider_visualization(value_data);
            eeprom_save_timer = timer_read();
            eeprom_save_pending = true;
            break;
        }
        case id_set_rapid_trigger_disengage_distance: {
            for (int i = 0; i < SENSOR_COUNT; i++) {
                he_key_rapid_trigger_configs[i].disengage_distance = value_data;
            }
            last_moved_slider = SLIDER_TYPE_RTP_DISENGAGE;
            current_slider_type = SLIDER_TYPE_RTP_DISENGAGE;
            slider_active = true;
            start_slider_visualization(value_data);
            eeprom_save_timer = timer_read();
            eeprom_save_pending = true;
            break;
        }
    }
    latest_slider_value = value_data;
}

void via_he_config_get_value(uint8_t *data) {
    uint8_t value_id   = data[0];
    uint8_t *value_data = &(data[1]);


    switch (value_id) {
        case id_via_he_actuation_threshold: {
            *value_data = he_key_configs[0].he_actuation_threshold;
            break;
        }
        case id_via_he_release_threshold: {
            *value_data = he_key_configs[0].he_release_threshold;
            break;
        }
        case id_start_calibration: {
            break;
        }
        case id_toggle_actuation_mode: {
            value_data[0] = he_config.he_actuation_mode;
            break;
        }
        case id_set_rapid_trigger_deadzone: {
            value_data[0] = he_key_rapid_trigger_configs[0].deadzone >> 8;
            value_data[1] = he_key_rapid_trigger_configs[0].deadzone & 0xFF;
            break;
        }
        case id_set_rapid_trigger_engage_distance: {
            value_data[0] = he_key_rapid_trigger_configs[0].engage_distance & 0xFF;
            break;
        }
        case id_set_rapid_trigger_disengage_distance: {
            value_data[0] = he_key_rapid_trigger_configs[0].disengage_distance & 0xFF;
            break;
        }
        default:
            /* Do nothing */
    }
}

void via_he_calibration_save(void) {
    eeconfig_update_user_datablock(&eeprom_he_key_configs);
}

// todo remove - unused
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
                // Bypass
                break;
            }
            case id_save_threshold_data: {
                for (int i = 0; i < SENSOR_COUNT; i++) {
                    eeprom_he_key_configs[i].he_actuation_threshold = via_he_key_configs[i].he_actuation_threshold;
                    eeprom_he_key_configs[i].he_release_threshold = via_he_key_configs[i].he_release_threshold;
                }
                break;
            }
            default: {
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }

    *command_id = id_unhandled;
}
