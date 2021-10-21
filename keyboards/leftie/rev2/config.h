
/* USB Device descriptor parameter */
#define VENDOR_ID       0x5363 // Sc for Smollchungus
#define PRODUCT_ID      0x0001
#define DEVICE_VER      0x0002
#define MANUFACTURER    smollchungus
#define PRODUCT         LeftieRev1
#define DESCRIPTION     left side of an alice

/* key matrix size */
#define MATRIX_ROWS 5
#define MATRIX_COLS 8

/* key matrix pins
   are in subfolders v1/v2 */

#define MATRIX_ROW_PINS { B2, B6, D7, E6, B4 }
#define MATRIX_COL_PINS { D4, B5, F5, F6, F7, B1, B3, F4 }

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* Set 0 if debouncing isn't needed */
#define DEBOUNCING_DELAY 5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE

/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* key combination for command */
#define IS_COMMAND() ( \
    keyboard_report->mods == (MOD_BIT(KC_LSHIFT) | MOD_BIT(KC_RSHIFT)) \
)

/* prevent stuck modifiers */
#define PREVENT_STUCK_MODIFIERS

