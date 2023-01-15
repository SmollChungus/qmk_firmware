#include QMK_KEYBOARD_H

enum layers {
    _QWERTY,
    _LOWER,
    _RAISE,
    _SPECIAL,
};

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define SPECIAL MO(_SPECIAL)


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

[_QWERTY] = LAYOUT_all(
		KC_1,    KC_2, KC_3, KC_4, KC_5
),

[_LOWER] = LAYOUT_all(
		_______, _______, _______, _______, _______
),

[_RAISE] = LAYOUT_all(
		_______, _______, _______, _______, _______
),

[_SPECIAL] = LAYOUT_all(
		_______, _______, _______, _______, _______
)
};
