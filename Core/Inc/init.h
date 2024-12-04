#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "stm32f429xx.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

void GPIO_Ini();
void RCC_Ini();
void ITR_Ini();
void SysTick_Ini();

#endif