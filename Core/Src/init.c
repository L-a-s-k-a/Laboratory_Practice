#include "main.h"

/*
 * Светодиоды:
 * зеленый - прямое обращение к памяти
 * синий -  макросы
 * красный - библиотека CMSIS
 * внешний - библиотека CMSIS
 */
void GPIO_Ini(void)
{
    /* Тактирование GPIOA, GPIOB, GPIOC */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    (void)READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); /* Короткая задержка после включения тактирования */

    // Прямое обращение
    *(volatile uint32_t*)(0x40020400UL + 0x00UL) |= 0x1UL;    // MODER0 = 01, выход 
    *(volatile uint32_t*)(0x40020400UL + 0x04UL) &= ~0x1UL;   // OTYPER0 = 0, Push-Pull 
    *(volatile uint32_t*)(0x40020400UL + 0x08UL) |= 0x1UL;    // OSPEEDR0 = 01, средняя скорость 
    *(volatile uint32_t*)(0x40020400UL + 0x0CUL) &= ~0x3UL;   // PUPDR0 = 00, без подтяжки 

    // Макросы
    MY_SET_BITS(MY_GPIOB_MODER, MY_PIN7_MODE_OUT);
    MY_CLR_BITS(MY_GPIOB_OTYPER, MY_PIN7_OTYPE_MASK);
    MY_SET_BITS(MY_GPIOB_OSPEEDR, MY_PIN7_OSPEED_MID);
    MY_CLR_BITS(MY_GPIOB_PUPDR, MY_PIN7_PUPDR_MASK);

    // CMSIS
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14);

    // CMSIS
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODE3_0);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_3);
    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEEDR3_0);
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR3);

    Leds_Set(0); // В начале все светодиоды выключены
}

// Включает первые count светодиодов
void Leds_Set(uint8_t count)
{
    // LED1 - зеленый
    *(volatile uint32_t*)(0x40020400UL + 0x18UL) = (count >= 1) ? (1UL « 0) : (1UL « 16);

    // LED2 - синий
    MY_GPIOB_BSRR = (count >= 2) ? MY_PIN7_SET : MY_PIN7_RESET;

    // LED3 - красный
    WRITE_REG(GPIOB->BSRR, (count >= 3) ? GPIO_BSRR_BS14 : GPIO_BSRR_BR14);

    // LED4 - внешний
    WRITE_REG(GPIOA->BSRR, (count >= 4) ? GPIO_BSRR_BS3 : GPIO_BSRR_BR3);
}





