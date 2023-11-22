#include QMK_KEYBOARD_H

enum layers {
    _QWERTY,
    _LOWER,

};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define SPECIAL MO(_SPECIAL)


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_QWERTY] = LAYOUT(
		KC_1, KC_2, KC_3, KC_4, KC_5
),

[_LOWER] = LAYOUT(
		_______, _______, _______, _______, _______
)
};
