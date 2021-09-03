#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
    _FN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_all(
     KC_F13,   KC_F14,        KC_F19,    KC_F20,
     KC_F15,   KC_F16,        KC_F21,    KC_F22,
     KC_F17,   KC_F18,        KC_F23,    KC_F24,
               KC_MS_BTN_1,   KC_MS_BTN_3
  ),

    [_FN] = LAYOUT_all(
     KC_F13,   KC_F14,        KC_F19,    KC_F20,
     KC_F15,   KC_F16,        KC_F21,    KC_F22,
     KC_F17,   KC_F18,        KC_F23,    KC_F24,
               KC_MS_BTN_1,   KC_MS_BTN_3
    )

};
