/* Copyright 2022 Smoll Chungus (@smollchungus)
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

// keymap
#define LAYOUT_all( \
	k000, k010, k001, k011, k002, k012, k003, k013, k004, k014, k005, k015, k006, \
    k100, k110, k101, k111, k102, k112, k103, k113, k104, k114, k105, k115, k106, \
    k200, k210, k201, k211, k202, k212, k203, k213, k204, k214, k205, k215, k206, \
    k300, k310, k301, k311,             k303,             k314, k305, k315, k306  \
) { \
	{ k000, k001,  k002,  k003,  k004,  k005, k006  }, \
    { k010, k011,  k012,  k013,  k014,  k015, KC_NO }, \
    { k100, k101,  k102,  k103,  k104,  k105, k106  }, \
    { k110, k111,  k112,  k113,  k114,  k115, KC_NO }, \
    { k200, k201,  k202,  k203,  k204,  k205, k206  }, \
    { k210, k211,  k212,  k213,  k214,  k215, KC_NO }, \
    { k300, k301,  KC_NO, k303,  KC_NO, k305, k306  }, \
    { k310, k311,  KC_NO, KC_NO, k314,  k315, KC_NO }  \
}
