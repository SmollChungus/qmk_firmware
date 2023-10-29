// Copyright 2023 Matthijs Muller (@Smollchungus)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT

#pragma once

#define WS2812_PIO_USE_PPIO1

#define RGB_MATRIX_LED_COUNT 71

#define RGB_DISABLE_WHEN_USB_SUSPENDED

#define RGB_MATRIX_MAXIMUM_BRIGHTNESS 120
