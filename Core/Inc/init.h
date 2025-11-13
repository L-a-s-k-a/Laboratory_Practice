#include <stdint.h>

#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h" // Specific definitions for STM32F429 chip
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"  // Main CMSIS library for STM32F4

#ifndef INIT_H
#define INIT_H

#include "stm32f4xx.h"    // Main CMSIS library for STM32F4
#include "stm32f429xx.h"  // Specific definitions for STM32F429 chip

void GPIO_Ini(void);      // Function to initialize all GPIO pins
#endif
