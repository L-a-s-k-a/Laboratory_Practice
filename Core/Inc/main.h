#ifndef INIT_H
#define INIT_H
#include <stdint.h>
#include "../../CMSIS/Devices/stm32f4xx.h" 
#include "../../CMSIS/Devices/stm32f429xx.h"

// Синий светодиоид
#define MY_RCC_AHB1ENR        (*(volatile uint32_t*)(0x40023800UL + 0x30UL)) // Тактирование
#define MY_GPIOB_MODER        (*(volatile uint32_t*)(0x40020400UL + 0x00UL))
#define MY_GPIOB_OTYPER       (*(volatile uint32_t*)(0x40020400UL + 0x04UL))
#define MY_GPIOB_OSPEEDR      (*(volatile uint32_t*)(0x40020400UL + 0x08UL))
#define MY_GPIOB_PUPDR        (*(volatile uint32_t*)(0x40020400UL + 0x0CUL))
#define MY_GPIOB_BSRR         (*(volatile uint32_t*)(0x40020400UL + 0x18UL))

#define MY_PIN7_MODE_OUT      0x00004000UL   
#define MY_PIN7_OTYPE_MASK    0x00000080UL   
#define MY_PIN7_OSPEED_MID    0x00004000UL  
#define MY_PIN7_PUPDR_MASK    0x0000C000UL   
#define MY_PIN7_SET           0x00000080UL  
#define MY_PIN7_RESET         0x00800000UL 

#define MY_CLR_BITS(REG, MASK) ((REG) &= ~(MASK))
#define MY_SET_BITS(REG, MASK) ((REG) |= (MASK))

// Кнопка
#define BTN_PRESSED()         (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0)

// Время антидребезга
#define DEBOUNCE_TICKS        40000UL

#define LEDS_COUNT            4U
#define SPECIAL_PRESS         6U            // Каждое 6е нажатие - весь ряд

void GPIO_Ini(void);
void Leds_Set(uint8_t count);               // Включить первые count светодиодов 

#endif