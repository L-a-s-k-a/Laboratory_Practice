#include "../Inc/init.h"

void GPIO_Ini(void)

{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN); // Включение тактирования портов GPIOB и GPIOC

    GPIOB_MODER |= GPIOB_MODE_PIN6_OUT;
    GPIOB_OTYPER |= GPIOB_OTYPE_PIN6_PP;
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN6_MID;
    GPIOB_PUPDR |= GPIOB_PUPDR_PIN6_NOPUPD;

    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x004000000UL; // Настройка работы 13-го пина GPIOB в режиме вывода сигнала (Output mode)
    *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x00;          // Настройка на Push-Pull работу 13-го пина GPIOB (Output Push-Pull)
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x001000000UL; // Настройка скорости работы 13-го пина GPIOB на среднюю
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x00;          // Отключение PU/PD резисторов для 13-го пина GPIOB

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE12_0);         // Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_12);        // Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR12_0); // Настройка скорости работы 14-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD12_0);       // Отключение PU/PD резисторов для 14-го пина GPIOB

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE15_0);         // Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_15);        // Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR15_0); // Настройка скорости работы 14-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD15_0);
}