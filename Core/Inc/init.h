#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"

void delay(volatile uint32_t count);
void GPIO_Init_with_memory_for_led5(void);
void GPIO_Init_with_my_macro(void);
void GPIO_Init_with_CMSIS(void);

// --- ПРЯМОЕ ОБРАЩЕНИЕ (для PA5 - светодиод LED1) ---
#define ADDR_RCC_AHB1ENR  ((volatile uint32_t*)0x40023830UL)
#define ADDR_GPIOA_MODER  ((volatile uint32_t*)0x40020000UL)
#define ADDR_GPIOA_OTYPER ((volatile uint32_t*)0x40020004UL)
#define ADDR_GPIOA_OSPEEDR ((volatile uint32_t*)0x40020008UL)
#define ADDR_GPIOA_PUPDR  ((volatile uint32_t*)0x4002000CUL)
#define ADDR_GPIOA_BSRR   ((volatile uint32_t*)0x40020018UL)

// --- СОБСТВЕННЫЕ МАКРОСЫ (для PA10 и PA11 - внешние светодиоды) ---
// Тактирование
#define RCC_GPIO          (*(volatile uint32_t*)(0x40023830UL))
#define RCC_GPIOA_ON      (1UL << 0)
#define RCC_GPIOB_ON      (1UL << 1)

// Регистры GPIOA для макросов
#define GPIOA_MODER       (*(volatile uint32_t*)(0x40020000UL))
#define GPIOA_OTYPER      (*(volatile uint32_t*)(0x40020004UL))
#define GPIOA_OSPEEDR     (*(volatile uint32_t*)(0x40020008UL))
#define GPIOA_PUPDR       (*(volatile uint32_t*)(0x4002000CUL))
#define GPIOA_BSRR        (*(volatile uint32_t*)(0x40020018UL))

// Регистры GPIOB для кнопок
#define GPIOB_MODER       (*(volatile uint32_t*)(0x40020400UL))
#define GPIOB_PUPDR       (*(volatile uint32_t*)(0x4002040CUL))
#define GPIOB_IDR         (*(volatile uint32_t*)(0x40020410UL))

// Бит-маски для PA10 и PA11
#define GPIOA_MODE_PIN10_OUT (0x1UL << (10 * 2))
#define GPIOA_MODE_PIN11_OUT (0x1UL << (11 * 2))
#define GPIOA_OTYPE_PIN10_PP (0x0UL << 10)
#define GPIOA_OTYPE_PIN11_PP (0x0UL << 11)
#define GPIOA_OSPEED_PIN10_HS (0x2UL << (10 * 2))
#define GPIOA_OSPEED_PIN11_HS (0x2UL << (11 * 2))
#define GPIOA_PUPD_PIN10_NOPUPD (0x0UL << (10 * 2))
#define GPIOA_PUPD_PIN11_NOPUPD (0x0UL << (11 * 2))

// Бит-маски для кнопок
#define GPIOB_MODE_PIN1_IN (0x0UL << (1 * 2))
#define GPIOB_MODE_PIN2_IN (0x0UL << (2 * 2))
#define GPIOB_MODE_PIN3_IN (0x0UL << (3 * 2))
#define GPIOB_MODE_PIN8_IN (0x0UL << (8 * 2))
#define GPIOB_PUPD_PIN1_PULL_UP (0x1UL << (1 * 2))
#define GPIOB_PUPD_PIN2_PULL_UP (0x1UL << (2 * 2))
#define GPIOB_PUPD_PIN3_PULL_UP (0x1UL << (3 * 2))
#define GPIOB_PUPD_PIN8_PULL_UP (0x1UL << (8 * 2))

// Макросы управления светодиодами PA10 и PA11
#define SET_GPIO_A10      (GPIOA_BSRR = (1UL << 10))
#define RESET_GPIO_A10    (GPIOA_BSRR = (1UL << 26))
#define SET_GPIO_A11      (GPIOA_BSRR = (1UL << 11))
#define RESET_GPIO_A11    (GPIOA_BSRR = (1UL << 27))

// Чтение кнопок через собственные макросы
#define READ_GPIO_B1      (GPIOB_IDR & (1UL << 1))
#define READ_GPIO_B2      (GPIOB_IDR & (1UL << 2))
#define READ_GPIO_B3      (GPIOB_IDR & (1UL << 3))
#define READ_GPIO_B8      (GPIOB_IDR & (1UL << 8))

// Макросы управления через CMSIS (для сравнения)
#define CMSIS_LED_PA10_ON()     (GPIOA->BSRR = GPIO_BSRR_BS_10)
#define CMSIS_LED_PA10_OFF()    (GPIOA->BSRR = GPIO_BSRR_BR_10)
#define CMSIS_LED_PA11_ON()     (GPIOA->BSRR = GPIO_BSRR_BS_11)
#define CMSIS_LED_PA11_OFF()    (GPIOA->BSRR = GPIO_BSRR_BR_11)
#define CMSIS_READ_BUTTON_B1()  (GPIOB->IDR & GPIO_IDR_IDR_1)
#define CMSIS_READ_BUTTON_B2()  (GPIOB->IDR & GPIO_IDR_IDR_2)
#define CMSIS_READ_BUTTON_B3()  (GPIOB->IDR & GPIO_IDR_IDR_3)
#define CMSIS_READ_BUTTON_B8()  (GPIOB->IDR & GPIO_IDR_IDR_8)

#endif
