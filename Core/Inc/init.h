#ifndef INIT_H
#define INIT_H
#include <stdint.h>
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "stm32f4xx.h"

void Clock_Init_HSE_PLL_168MHz(void);
void SysTick_init_1mc(void);          

void LEDs_GPIO_Init(void);
void Buttons_GPIO_Init(void);

void Buttons_EXTI_Init(void);



#endif

