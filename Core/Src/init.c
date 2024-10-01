#include "../Inc/init.h"

void GPIO_Ini(void)
{
    RCC_GPIO_EN |= 0x02; //Включение тактирования портов GPIOB и GPIOC
    GPIOB_MODER |= 0x4000; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    GPIOB_OTYPER |= 0x00; //Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
    GPIOB_OSPEEDR |= 0x4000; //Настройка скорости работы 7-го пина GPIOB на среднюю
    GPIOB_PUPDR |= 0x00; //Отключение PU/PD резисторов для 7-го пина GPIOB
}