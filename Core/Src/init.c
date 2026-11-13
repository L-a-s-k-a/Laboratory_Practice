#include "init.h"

void GPIO_Ini(void) {
    // ючение тактирования портов GPIOB и GPIOC через CMSISВкл
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    /* 
       1. Настройка LED 1 (PB0) - ПРЯМЫМ ОБРАЩЕНИЕМ К ПАМЯТИ
        */
    *(volatile uint32_t*)(0x40020400UL + 0x00UL) |= (0x01UL << 0);
    *(volatile uint32_t*)(0x40020400UL + 0x04UL) &= ~(0x01UL << 0);
    *(volatile uint32_t*)(0x40020400UL + 0x08UL) |= (0x01UL << 0);
    *(volatile uint32_t*)(0x40020400UL + 0x0CUL) &= ~(0x03UL << 0);

    /* 
       2. Настройка LED 2 (PB7) - СОБСТВЕННЫМИ МАКРОСАМИ
       */
    MY_SET_BIT(MY_GPIOB_MODER, MY_PIN7_MODE_OUT);
    MY_CLEAR_BIT(MY_GPIOB_OTYPER, (1UL << 7));
    MY_SET_BIT(MY_GPIOB_OSPEEDR, MY_PIN7_OSPEED_MID);
    MY_CLEAR_BIT(MY_GPIOB_PUPDR, (3UL << 14));

    /* 
       3. Настройка LED 3..6 (PB14, PB1, PB2, PB15) - ЧЕРЕЗ CMSIS
       */
    GPIOB->MODER |= (GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | 
                     GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0);

    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 | 
                       GPIO_OTYPER_OT14 | GPIO_OTYPER_OT15);

    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR1_0 | GPIO_OSPEEDER_OSPEEDR2_0 | 
                       GPIO_OSPEEDER_OSPEEDR14_0 | GPIO_OSPEEDER_OSPEEDR15_0);

    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD2 | 
                      GPIO_PUPDR_PUPD14 | GPIO_PUPDR_PUPD15);

    GPIOC->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | 
                      GPIO_MODER_MODER3 | GPIO_MODER_MODER13);

    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD1 | GPIO_PUPDR_PUPD3);
    GPIOC->PUPDR |= (GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD1_1 | GPIO_PUPDR_PUPD3_1);
}