#include "init.h"

void GPIO_Ini(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); // Включение тактирования портов GPIOB и GPIOC

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER7_0);        // Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);        // Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0); // Настройка скорости работы 7-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD7_0);       // Отключение PU/PD резисторов для 7-го пина GPIOB
}