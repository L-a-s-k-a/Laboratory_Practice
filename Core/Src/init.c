#include "../Inc/init.h"

void GPIO_Ini(void)
{
    *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02; //Включение тактирования портов GPIOB и GPIOC
    *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; //Настройка скорости работы 7-го пина GPIOB на среднюю
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00; //Отключение PU/PD резисторов для 7-го пина GPIOB
}