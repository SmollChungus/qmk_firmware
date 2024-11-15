//mcuconf.h

#pragma once

#include_next <mcuconf.h>

//#undef STM32_PWM_USE_TIM1 //ifdef rgbled
//#define STM32_PWM_USE_TIM1 TRUE //was TIM3 for PB1 setup

//#undef STM32_PWM_USE_ADVANCED
//#define STM32_PWM_USE_ADVANCED TRUE // for PA8

#undef STM32_ADC_USE_ADC1
#define STM32_ADC_USE_ADC1 TRUE

