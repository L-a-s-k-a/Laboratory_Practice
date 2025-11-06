#pragma once

#define RCC_BASE        0x40023800UL
#define RCC_AHB1ENR     (*(volatile unsigned long *)(RCC_BASE + 0x30))

#define GPIOC_EN        (1U << 2)
#define GPIOD_EN        (1U << 3)
#define GPIOE_EN        (1U << 4)

#define ENABLE_GPIOC_CLOCK()    (RCC_AHB1ENR |= GPIOC_EN)
#define ENABLE_GPIOD_CLOCK()    (RCC_AHB1ENR |= GPIOD_EN)
#define ENABLE_GPIOE_CLOCK()    (RCC_AHB1ENR |= GPIOE_EN)

void enableClockC(void);
void enableClockD(void);
void enableClockE(void);