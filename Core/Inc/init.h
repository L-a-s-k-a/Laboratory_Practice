#ifndef INIT_H
#define INIT_H

#include "stm32f4xx.h"
#include <stdint.h>

/* Тактирование: HSE + PLL = 168 МГц */
void Clock_Init_HSE_PLL_168MHz(void);

/* SysTick с периодом 1 мс */
void SysTick_Init_1ms(void);

/* Инициализация GPIO (PF7, PF8, PF9, PG1 + PA0) и EXTI0 */
void GPIO_EXTI_Init(void);

/* Инициализация TIM2 (обычный счётчик, прерывание по обновлению)
   ВНИМАНИЕ: таймер на выходе ИНИЦИАЛИЗИРОВАН, НО НЕ ЗАПУЩЕН (CEN = 0). */
void TIM2_Init(void);

/* Обновление частоты TIM2.
   index: 0..3 -> 1 Гц, 5 Гц, 10 Гц, 20 Гц. */
void TIM2_UpdateFrequency(uint8_t index);

/* Управление светодиодами */
void LEDs_AllOff(void);
void LED_OnIndex(uint8_t index);

#endif
