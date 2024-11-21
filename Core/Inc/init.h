#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "..\..\CMSIS\Devices\stm32f429xx.h"
#include "..\..\CMSIS\Devices\stm32f4xx.h"
#include "..\..\CMSIS\Devices\system_stm32f4xx.h"

void GPIO_Ini();
void RCC_Ini();
void ITR_Ini();
void SysTick_Ini();

#endif