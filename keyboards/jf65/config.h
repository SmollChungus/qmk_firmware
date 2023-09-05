/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

// detect handedness by EEPROM, <qmk flash -kb <keyboard> -km <keymap> -bl <uf2-split-left and uf2-split-right for each side, numpad is considered right, main 65% left>
#define EE_HANDS

// RGB
//#define RGBLED_NUM 53
//#define RGBLIGHT_SPLIT
//#define RGBLED_SPLIT { 39, 14 }

#define WS2812_PIO_USE_PIO1 // Force the usage of PIO1 peripheral, by default the WS2812 implementation uses the PIO0 peripheral


//Communication
// #define SPLIT_CONNECTION_CHECK_TIMEOUT 500

/* Optional Data Sync
#define SPLIT_TRANSPORT_MIRROR
#define SPLIT_MODS_ENABLE
*/
