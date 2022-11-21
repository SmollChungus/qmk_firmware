/* Copyright 2018-2022 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-3.0-or-later
#include <stdio.h>
#include QMK_KEYBOARD_H
#include "analog.h"
#include "qp.h"
#include "qp_ssd1351.h"

#include "rgboledtest.qgf.c"

static painter_device_t oled;

void ui_init(void) {
oled = qp_ssd1351_make_spi_device(128, 128, OLED_CS_PIN, OLED_DC_PIN, OLED_RST_PIN, 8, 0);
}

bool qp_init(painter_device_t, QP_ROTATION_0);

void keyboard_post_init_kb(void) {
    display = qp_make_dotdotdot;
    qp_init(painter_device_t, QP_ROTATION_0);
}

bool qp_power(painter_device_t device, bool power_on);

*/

