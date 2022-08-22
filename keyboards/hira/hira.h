/*
Copyright 2022 hiraworks

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "quantum.h"

#define kxx KC_NO

/*
 *                                                                  ┌───────┐
 *                                                     2u Backspace │0d     │
 *                                                                  └───────┘
 *              ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐ ┌───┬───┬───┐
 *              │00 │01 │02 │03 │04 │05 │06 │07 │08 │09 │0a │0b │0c │0d │2d │ │0e │0f │0g │
 *              ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┤ ├───┼───┼───┤        ┌─────┐
 *              │10   │11 │12 │13 │14 │15 │16 │17 │18 │19 │1a │1b │1c │1d   │ │1e │1f │1g │        │     │ ISO
 *  2.25u       ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴─────┤ └───┴───┴───┘     ┌──┴┐2c  │ Enter
 *  LShift      │20    │21 │22 │23 │24 │25 │26 │27 │28 │29 │2a │2b │2c      │                   │1d │    │
 * ┌────────┐   ├────┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴────┬───┤     ┌───┐       ┌─┴───┴────┤
 * │30      │   │30  │41 │31 │32 │33 │34 │35 │36 │37 │38 │39 │3a │3b    │3d │     │3f │       │3b        │ 2.75u RShift
 * └────────┘   ├────┼───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴───┼───┴┬────┬┴───┤ ┌───┼───┼───┐   └──────────┘
 *              │40  │42  │43  │           45           │49  │4a  │4b  │4d  │ │4e │4f │4g │
 *              └────┴────┴────┴────────────────────────┴────┴────┴────┴────┘ └───┴───┴───┘
 *              ┌─────┬───┬─────┬───────────────────────────┬─────┬───┬─────┐
 *              │40   │42 │43   │            45             │4a   │4b │4d   │ Tsangan
 *              └─────┴───┴─────┴───────────────────────────┴─────┴───┴─────┘
 */

#define LAYOUT_all( \
    k000, k001, k002, k003, k004, k005, k006, k007, k008, k009, k010, k011, k012, k013, k014, k015, k016, k017,  \
    k100, k101, k102, k103, k104, k105, k106, k107, k108, k109, k110, k111, k112,       k114, k115, k116, k117,  \
    k200, k201, k202, k203, k204, k205, k206, k207, k208, k209, k210, k211, k212,       k214,                    \
    k300, k301, k302, k303, k304, k305, k306, k307, k308, k309, k310, k311, k312,       k314,       k316,        \
    k400, k401, k402,                k406,                k409, k410, k411, k412,             k415, k416, k417   \
) { \
    { k000, k001, k002, k003, k004, k005, k006, k007, k008, k009, k010, k011, k012, k013, k014, k015, k016, k017 }, \
    { k100, k101, k102, k103, k104, k105, k106, k107, k108, k109, k110, k111, k112, kxx,  k114, k115, k116, k117 }, \
    { k200, k201, k202, k203, k204, k205, k206, k207, k208, k209, k210, k211, k212, kxx,  k214, kxx,  kxx,  kxx }, \
    { k300, k301, k302, k303, k304, k305, k306, k307, k308, k309, k310, k311, k312, kxx,  k314, kxx,  k316, kxx }, \
    { k400, k401, k402, kxx,  kxx,  kxx,  k406, kxx,  kxx,  k409, k410, k411, k412, kxx,  kxx,  k415, k416, k417 } \
}

