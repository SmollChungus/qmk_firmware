#pragma once

#include "config_common.h"



/* USB Device descriptor parameter */
#define VENDOR_ID       0x5363 // Sc for Smollchungus
#define PRODUCT_ID      0x0002
#define DEVICE_VER      0x0001
#define MANUFACTURER    smollchungus
#define PRODUCT         acidalia

/* Key matrix size */
#define MATRIX_ROWS 4
#define MATRIX_COLS 4

/* Laser sensor unit */
#define ADNS_9800

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW


/* Set 0 if debouncing isn't needed */
#define DEBOUNCING_DELAY 5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLEOCKING_SUPP

/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION

/* Optical sensor settings */
#define SCROLL_DIVIDER 12
#define CPI_1 2000
#define CPI_2 4000
#define CPI_3 8000


/* prevent stuck modifiers */
#define PREVENT_STUCK_MODIFIERS

/* disable these deprecated features by default */
#define NO_ACTION_MACRO
#define NO_ACTION_FUNCTION
