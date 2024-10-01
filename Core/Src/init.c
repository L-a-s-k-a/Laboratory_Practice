#include "../Inc/init.h"

void GPIO_Ini(void)
{
    RCC_GPIO_EN |= RCC_GPIOB_EN + RCC_GPIOC_EN; //Включение тактирования портов GPIOB и GPIOC
    GPIOB_MODER |= GPIOB_MODE_PIN7_OUT; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    GPIOB_OTYPER |= GPIOB_OTYPE_PIN7_PP; //Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN7_MID; //Настройка скорости работы 7-го пина GPIOB на среднюю
    GPIOB_PUPDR |= GPIOB_PUPDR_PIN7_NOPUPD; //Отключение PU/PD резисторов для 7-го пина GPIOB
}