#ifndef INIT_H
#define INIT_H

#include "stm32f4xx.h"
#include "stm32f429xx.h"

#define RCC_AHB1ENR (*(volatile uint32_t *)(0x40023800UL + 0x30UL))
#define GPIOG_BASE  0x40021800UL
#define GPIOF_BASE  0x40021400UL
#define GPIOE_BASE  0x40021000UL
#define GPIOD_BASE  0x40020C00UL

#define GPIOG_MODER   (*(volatile uint32_t *)(GPIOG_BASE + 0x00UL))
#define GPIOG_OTYPER  (*(volatile uint32_t *)(GPIOG_BASE + 0x04UL))
#define GPIOG_OSPEEDR (*(volatile uint32_t *)(GPIOG_BASE + 0x08UL))
#define GPIOG_PUPDR   (*(volatile uint32_t *)(GPIOG_BASE + 0x0CUL))
#define GPIOG_BSRR    (*(volatile uint32_t *)(GPIOG_BASE + 0x18UL))

#define GPIOF_MODER   (*(volatile uint32_t *)(GPIOF_BASE + 0x00UL))
#define GPIOF_PUPDR   (*(volatile uint32_t *)(GPIOF_BASE + 0x0CUL))
#define GPIOF_IDR     (*(volatile uint32_t *)(GPIOF_BASE + 0x10UL))
#define GPIOF_BSRR    (*(volatile uint32_t *)(GPIOF_BASE + 0x18UL))

#define GPIOE_MODER   (*(volatile uint32_t *)(GPIOE_BASE + 0x00UL))
#define GPIOE_BSRR    (*(volatile uint32_t *)(GPIOE_BASE + 0x18UL))

#define GPIOD_MODER   (*(volatile uint32_t *)(GPIOD_BASE + 0x00UL))
#define GPIOD_PUPDR   (*(volatile uint32_t *)(GPIOD_BASE + 0x0CUL))
#define GPIOD_IDR     (*(volatile uint32_t *)(GPIOD_BASE + 0x10UL))
#define GPIOD_BSRR    (*(volatile uint32_t *)(GPIOD_BASE + 0x18UL))

#define GPIOG_PIN9  (1UL << 9)  // D0
#define GPIOG_PIN14 (1UL << 14) // D1
#define GPIOF_PIN15 (1UL << 15) // D2
#define GPIOE_PIN13 (1UL << 13) // D3
#define GPIOF_PIN14 (1UL << 14) // D4
#define GPIOE_PIN11 (1UL << 11) // D5
#define GPIOF_PIN12 (1UL << 12) // D8
#define GPIOD_PIN15 (1UL << 15) // D9

void GPIO_Init(void);

#endif
