//he_eeprom.h
#pragma once

#include "eeprom.h"

#if (EECONFIG_KB_DATA_SIZE) > 0
#    define EEPROM_KB_PARTIAL_UPDATE(__struct, __field) eeprom_update_block(&(__struct.__field), (void *)((void *)(EECONFIG_KB_DATABLOCK) + offsetof(typeof(__struct), __field)), sizeof(__struct.__field))
#endif

#if (EECONFIG_USER_DATA_SIZE) > 0
#    define EEPROM_USER_PARTIAL_UPDATE(__struct, __field) eeprom_update_block(&(__struct.__field), (void *)((void *)(EECONFIG_USER_DATABLOCK) + offsetof(typeof(__struct), __field)), sizeof(__struct.__field))
#endif
