#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID       0x04D8
#define PRODUCT_ID      0xEDB5
#define DEVICE_VER      0x0001
#define MANUFACTURER    CLACK-ATTACK
#define PRODUCT         nice69

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 15

// ROWS: Top to bottom, COLS: Left to right

#define MATRIX_ROW_PINS { F0, F1, C7, D6, B7 }
#define MATRIX_COL_PINS { F4, F5, F6, F7, B1, B3, B2, B6, B5, B4, E6, D7, C6, D4, D0}
#define UNUSED_PINS

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* define if matrix has ghost */
//#define MATRIX_HAS_GHOST

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define LED_CAPS_LOCK_PIN D2
#define LED_PIN_ON_STATE 1

/* Backlight configuration */
#define BACKLIGHT_PIN B0
#define BACKLIGHT_BREATHING
#define BACKLIGHT_LEVELS 4

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
//#define NO_ACTION_MACRO
//#define NO_ACTION_FUNCTION


#define VIAL_KEYBOARD_UID {0x99, 0x5E, 0x50, 0x8C, 0x22, 0xEA, 0xD2, 0xAD}
#define VIAL_UNLOCK_COMBO_ROWS { 0, 3 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 0 }
