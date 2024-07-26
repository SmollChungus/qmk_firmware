
//matrix.c
#include "he_switch_matrix.h" //instead of instead of #include "matrix.h"
#include "wait.h"
#include "print.h"

/* matrix state(1:on, 0:off) */
matrix_row_t raw_matrix[MATRIX_ROWS]; // raw values
matrix_row_t matrix[MATRIX_ROWS];     // debounced values

uint8_t console_output = 0;

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }
__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }
__attribute__((weak)) void matrix_init_user(void) {}
__attribute__((weak)) void matrix_scan_user(void) {}



void matrix_print(void) {
    he_matrix_print();
}

void matrix_init(void) {

    he_init(he_key_configs, SENSOR_COUNT);

    //dummy call
    matrix_init_kb();

    wait_ms(5);

    noise_floor_calibration_init();

    matrix_scan_kb();
}

uint8_t matrix_scan(void) {
    int updated = he_matrix_scan();  // Updated function call and type

    if (console_output == 0) { //for web app
        /*do nothing*/
    }
    else if (console_output == 1) { //for web app
        static int cnt = 0;
        if (cnt++ == 5) {
            cnt = 0;
            he_matrix_print();
        }
    }
    else if (console_output == 2) { //for debugging
        static int cnt2 = 0;
        if (cnt2++ == 5000) {
            cnt2 = 0;
            he_matrix_print_extended();
        }
    }
    else if (console_output == 3) { //both but slow
        static int cnt = 0;
        static int cnt2 = 0;
        if (cnt++ == 500) {
            cnt = 0;
            he_matrix_print();
        }
        if (cnt2++ == 1000) {
            cnt2 = 0;
            he_matrix_print_extended();
        }
    }
    else if (console_output == 4) { //rapid trigger web app
        static int cnt2 = 0;
        if(cnt2++ == 2000) {
            cnt2 = 0;
            he_matrix_print_rapid_trigger();
        }
    }
    else if (console_output == 5) { //0,0 escape key algorithm debug
        he_matrix_print_rapid_trigger_debug();
    }

    return updated ? 1 : 0;
}
