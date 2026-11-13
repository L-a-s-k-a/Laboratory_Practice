#ifndef INIT_H_
#define INIT_H_

#include <stdint.h>
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h"

//Собственные директивы для LED2 
#define MY_GPIOB_MODER          (*(volatile uint32_t*)(0x40020400UL + 0x00UL))
#define MY_GPIOB_OTYPER         (*(volatile uint32_t*)(0x40020400UL + 0x04UL))
#define MY_GPIOB_OSPEEDR        (*(volatile uint32_t*)(0x40020400UL + 0x08UL))
#define MY_GPIOB_PUPDR          (*(volatile uint32_t*)(0x40020400UL + 0x0CUL))

#define MY_PIN7_MODE_OUT        (0x01UL << 14) 
#define MY_PIN7_OSPEED_MID      (0x01UL << 14) 

// Собственные макросы
#define MY_SET_BIT(REG, BIT)    ((REG) |= (BIT))
#define MY_CLEAR_BIT(REG, BIT)  ((REG) &= ~(BIT))

void GPIO_Ini(void);

#endif 