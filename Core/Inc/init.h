#ifndef INIT_H
#define INIT_H

#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"

void RCC_Init(void);
void GPIO_Init(void);
void EXTI_ITR_Init(void); 
void SysTick_Init(void); 

#endif // INIT_H



