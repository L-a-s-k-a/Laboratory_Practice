#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "init.h"


#define BTN_DEBOUNCE_MS 200U

//SysTick — вызывается каждые 1 мс (настройка в SysTick_Init_1ms) 
void SysTick_Handler(void);

//EXTI0 — кнопка 1 на PA0 (смена режима работы гирлянды)
void EXTI0_IRQHandler(void);

//EXTI9_5 — кнопка 2 (PA5, смена частоты) и кнопка 3 (PA6, смена номера светодиода)
void EXTI9_5_IRQHandler(void);

#endif
