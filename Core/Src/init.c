#include "init.h"

void GPIO_Ini(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); // Включение тактирования портов GPIOB и GPIOC
/*
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER7_0);        // Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);        // Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0); // Настройка скорости работы 7-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD7_0);       // Отключение PU/PD резисторов для 7-го пина GPIOB
*/
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER14_0);        // Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);        // Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0); // Настройка скорости работы 14-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD14_0);       // Отключение PU/PD резисторов для 14-го пина GPIOB

    /*SET_BIT(GPIOB->MODER, GPIO_MODER_MODER0_0);        // Настройка работы 0-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);        // Настройка на Push-Pull работу 0-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0); // Настройка скорости работы 0-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD0_0);       // Отключение PU/PD резисторов для 0-го пина GPIOB
    */

(*(uint32_t*)(0x40020400UL + 0x00UL)) |= 0x004000UL; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
(*(uint32_t*)(0x40020400UL + 0x04UL)) |=0x000000UL; //Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
(*(uint32_t*)(0x40020400UL + 0x08UL)) |= 0x004000UL; //Настройка скорости работы 7-го пина GPIOB на среднюю
(*(uint32_t*)(0x40020400UL + 0x0CUL)) |= 0x000000UL; //Отключение PU/PD резисторов для 7-го пина GPIOB
}