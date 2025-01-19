#ifndef IT_HANDLERS_H
#define IT_HANDLERS_H

#include "init.h"

#define DELAY_BUTTON_FILTER 50

void SysTick_Handler(void);
void EXTI15_10_IRQHandler(void);

#endif