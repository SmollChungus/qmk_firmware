#include "leftie.h"

#include QMK_KEYBOARD_H

#define _QWERTY 0
#define _LOWER 1
#define _RAISE 2
#define _SPECIAL 3

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define SPECIAL MO(_SPECIAL)





const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {



[_QWERTY] = LAYOUT_all(
		KC_UP,    KC_ESC,   KC_1,    KC_2,  KC_3,  KC_4, KC_5, KC_6, \
		KC_DOWN,  KC_TAB,   KC_Q,    KC_W,  KC_E,  KC_R, KC_T, \
		KC_RGHT,  LOWER,    KC_A,    KC_S,  KC_D,  KC_F, KC_G, \
		KC_LSFT,  KC_Z,     KC_X,    KC_C,  KC_V,  KC_B, \
		KC_LCTL,  KC_LALT,  RAISE,   SPECIAL \
),

[_LOWER] = LAYOUT_all(
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_SPC, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS \
),

[_RAISE] = LAYOUT_all(
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS \
),

[_SPECIAL] = LAYOUT_all(
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
		KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS \
)
};

#ifdef OLED_ENABLE
void oled_task_user(void) {
    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);

    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("Default\n"), false);
            break;
        case _LOWER:
            oled_write_P(PSTR("LOWER\n"), false);
            break;
        case _RAISE:
            oled_write_P(PSTR("RAISE\n"), false);
            break;
        case _SPECIAL:
            oled_write_P(PSTR("SPECIAL\n"), false);
            break;
        default:
            // Or use the write_ln shortcut over adding '\n' to the end of your string
            oled_write_ln_P(PSTR("Undefined"), false);
    }

}
#endif
