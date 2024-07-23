
//matrix.c
#include "he_switch_matrix.h" //instead of instead of #include "matrix.h"
#include "wait.h"
#include "print.h"

/* matrix state(1:on, 0:off) */
matrix_row_t raw_matrix[MATRIX_ROWS]; // raw values
matrix_row_t matrix[MATRIX_ROWS];     // debounced values

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

bool matrix_scan(matrix_row_t current_matrix[]) {
    bool updated = he_matrix_scan();

    #if CONSOLE_VERBOSITY == 1  //for web app
    static int cnt = 0;

    if (cnt++ == 5) {
        cnt = 0;
        he_matrix_print();
    }
    #endif
    #if CONSOLE_VERBOSITY == 2 //for debugging
    static int cnt2 = 0;
    if(cnt2++ == 5000) {
        cnt2 = 0;
        he_matrix_print_extended();
    }
    #endif
    #if CONSOLE_VERBOSITY == 3 //both but slow
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
    #endif
    #if CONSOLE_VERBOSITY == 4 //rapid trigger web app
    static int cnt2 = 0;
    if(cnt2++ == 2000) {
        cnt2 = 0;
        he_matrix_print_rapid_trigger();
    }
    #endif
    #if CONSOLE_VERBOSITY == 5 //0,0 escape key algorithm debug
    he_matrix_print_rapid_trigger_debug();
    #endif
    #ifdef RAW_ENABLE
    static int report_cnt = 0;
    if (report_cnt++ == 15000) {
        report_cnt = 0;
        send_matrix_state_report(); // Send the matrix state report
        for (uint8_t report_id = 0; report_id < NUM_SENSOR_REPORTS; report_id++) {
            uint8_t start_sensor = report_id * SENSORS_PER_REPORT;
            send_sensor_value_report(report_id, start_sensor);
        }
    }


    #endif

    return updated ? 1 : 0;
}
