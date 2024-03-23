#ifdef RAW_ENABLE

#define MATRIX_STATE_REPORT_ID 0x01
#define SENSOR_VALUE_REPORT_ID_BASE 0x02 // Base for sensor value reports

void send_matrix_state_report(void) {
    uint8_t report[MATRIX_STATE_REPORT_SIZE] = {MATRIX_STATE_REPORT_ID};
    // Populate report[1] to report[9] with key states
    raw_hid_send(report, sizeof(report));
}

void send_sensor_value_report(uint8_t report_number, uint8_t start_sensor) {
    uint8_t report[SENSOR_REPORT_SIZE] = {SENSOR_VALUE_REPORT_ID_BASE + report_number};
    // Populate report with sensor values starting from 'start_sensor'
    for (int i = 0; i < SENSORS_PER_REPORT; ++i) {
        uint16_t sensor_value = read_sensor_value(start_sensor + i);
        // Assuming function to read sensor value returns 0-1023
        report[2 * i + 1] = (sensor_value >> 8) & 0xFF; // High byte
        report[2 * i + 2] = sensor_value & 0xFF; // Low byte
    }
    raw_hid_send(report, sizeof(report));
}
#endif
