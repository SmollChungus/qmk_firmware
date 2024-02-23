//matrix.c
#include "he_switch_matrix.h"
//#include "matrix.h" moved to ^

/* matrix state(1:on, 0:off) */
matrix_row_t raw_matrix[MATRIX_ROWS]; // raw values
matrix_row_t matrix[MATRIX_ROWS];     // debounced values

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }
__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }
__attribute__((weak)) void matrix_init_user(void) {}
__attribute__((weak)) void matrix_scan_user(void) {}


void matrix_print(void) {
    he_print_matrix();
}


void matrix_init(void) {
    // Initialize default HE configuration
    he_config.actuation_threshold = DEFAULT_ACTUATION_LEVEL;
    he_config.release_threshold = DEFAULT_RELEASE_LEVEL;

    // Check if EEPROM is set up for this keyboard
    if (!eeconfig_is_enabled()) {
        eeconfig_init(); // Initialize the EEPROM with default values
        // Save default HE config to EEPROM here as well
        eeprom_update_block((const void*)&he_config, (void*)&eeprom_he_config, sizeof(he_config_t));
    } else {
        // Load existing config from EEPROM
        eeprom_read_block((void*)&he_config, (const void*)&eeprom_he_config, sizeof(he_config_t));
    }

    // Continue with matrix initialization
    he_init();
    matrix_init_kb();
    matrix_scan_kb();
}

bool matrix_scan(matrix_row_t current_matrix[]) {
    bool updated = he_matrix_scan(current_matrix);

    #ifdef CONSOLE_ENABLE
    static int cnt = 0;
    if (cnt++ == 350) {
        cnt = 0;
        he_print_matrix();
    }
    #endif

    return updated ? 1 : 0;
}


