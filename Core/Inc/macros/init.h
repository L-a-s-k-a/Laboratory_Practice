#ifndef LAB1_INIT_H
#define LAB1_INIT_H

#include <stdbool.h>
#include <stdint.h>

#define RCC_AHB1ENR     (*(volatile uint32_t *)0x40023830UL)
#define GPIOD_MODER     (*(volatile uint32_t *)0x40020C00UL)
#define GPIOD_OTYPER    (*(volatile uint32_t *)0x40020C04UL)
#define GPIOD_OSPEEDR   (*(volatile uint32_t *)0x40020C08UL)
#define GPIOD_PUPDR     (*(volatile uint32_t *)0x40020C0CUL)
#define GPIOD_BSRR      (*(volatile uint32_t *)0x40020C18UL)
#define GPIOE_MODER     (*(volatile uint32_t *)0x40021000UL)
#define GPIOE_OTYPER    (*(volatile uint32_t *)0x40021004UL)
#define GPIOE_OSPEEDR   (*(volatile uint32_t *)0x40021008UL)
#define GPIOE_PUPDR     (*(volatile uint32_t *)0x4002100CUL)
#define GPIOE_IDR       (*(volatile uint32_t *)0x40021010UL)
#define GPIOE_BSRR      (*(volatile uint32_t *)0x40021018UL)

#define CLOCKS          ((1UL << 3) | (1UL << 4))
#define E_MODES         ((3UL << 14) | (3UL << 16) | (3UL << 18) | (3UL << 20))
#define E_PUSH_PULL     ((1UL << 7) | (1UL << 8) | (1UL << 10))
#define SHARED_OPEN_DRAIN (1UL << 9)
#define BUTTON_PULLUPS  ((1UL << 14) | (1UL << 16) | (1UL << 18))
#define LED_SUPPLY_MODE   (3UL << 20)
#define LED_SUPPLY_OUTPUT (1UL << 20)
#define LED_SUPPLY_HIGH   (1UL << 10)
#define D_PINS          ((1UL << 13) | (1UL << 14) | (1UL << 15))
#define D_MODES         ((3UL << 26) | (3UL << 28) | (3UL << 30))
#define D_OUTPUTS       ((1UL << 26) | (1UL << 28) | (1UL << 30))
#define SHARED_LOW      (1UL << 25)
#define SHARED_MODE     (3UL << 18)
#define SHARED_OUTPUT   (1UL << 18)
#define INPUT_BUTTON    (1UL << 7)
#define OUTPUT_BUTTON   (1UL << 8)
#define SHARED_BUTTON   (1UL << 9)
#define RED_OFF         (1UL << 30)
#define BLUE_OFF        (1UL << 31)
#define ORANGE_OFF      (1UL << 29)
#define RED_ON          (1UL << 14)
#define BLUE_ON         (1UL << 15)
#define ORANGE_ON       (1UL << 13)

#define READ_BIT(REG, MASK) ((REG) & (MASK))
#define SET_BIT(REG, MASK) ((REG) |= (MASK))
#define CLEAR_BIT(REG, MASK) ((REG) &= ~(MASK))
#define WRITE_REG(REG, VALUE) ((REG) = (VALUE))

void GPIO_Ini(void);
void GPIO_SetSharedOutput(bool output);
bool GPIO_ReadInputButton(void);
bool GPIO_ReadOutputButton(void);
bool GPIO_ReadSharedButton(void);
void GPIO_SetIndicators(bool red, bool blue, bool orange);

#endif
