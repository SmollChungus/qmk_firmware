
#include "quantum.h"

#define FORCE_NKRO

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_kb(void) {
    if (!rgb_matrix_indicators_user()) {
        return false;
    }

    if (host_keyboard_led_state().caps_lock) {
        rgb_matrix_set_color(2, 200, 0, 0);
    } else {
        rgb_matrix_set_color(2, 0, 0, 0);
    }
    return true;
}
#endif
