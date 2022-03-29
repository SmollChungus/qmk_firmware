/* Copyright 2022 SmollChungus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "quantum.h"


#define LAYOUT_all( \
    K00, K01, K02, K03, K04, \
    K10, K11, K12,      K14, \
              K22,      K24, \
    K30,      K32, K33, K34, \
    K40,                K44  \
) \
{ \
    { K00,   K01,   K02,   K03,    K04 }, \
    { K10,   K11,   K12,   KC_NO,  K14 },  \
    { KC_NO, KC_NO, K22,   KC_NO,  K24 }, \
    { K30,   KC_NO, K32,   K33,    K34 }, \
    { K40,   KC_NO, KC_NO, KC_NO,  K44 }   \
}

#define LAYOUT layout
