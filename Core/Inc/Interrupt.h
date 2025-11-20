#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "stm32f4xx.h"
#include "init.h"
#include <stdint.h>

/* Время в мс от SysTick */
extern volatile uint32_t g_msTicks;

/* Текущий светодиод (0..3) */
extern volatile uint8_t g_currentLed;

/* Текущая скорость (0..3 -> 1,5,10,20 Гц) */
extern volatile uint8_t g_speedIndex;

/* Флаг: гирлянда запущена (0 - стоит, 1 - работает) */
extern volatile uint8_t g_running;

extern volatile uint32_t g_debugCounter;

#endif
