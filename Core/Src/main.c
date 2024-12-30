#include "init.h"

int main(void)
{
    RCC_GPIO_EN |= 0x06; // Включение тактирования порта GPIOB

    GPIOB_MODER |= GPIO_PIN_7_OUT; // Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)

    // *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x00;   // Настройка на PushPull работу 7-го пина GPIOB (Output Push-Pull) работы 7-го пина GPIOB на среднюю
    GPIOB_OSPEEDR |= GPIO_PIN_7_MED; // Настройка скорости

    GPIOB_PUPDR |= 0x00; // Отключение PU/PD резисторов для 7-го пина GPIOB

    while (1)
    {
        if ((GPIOC_IDR & (GPIO_PIN_13)) != 0)
        {
            GPIOB_BSRR |= GPIO_PIN_7_SET; // Установка единицы в 7-ой бит регистра ODR
        }
        else
        {
            GPIOB_BSRR |= GPIO_PIN_7_RESET; // Установка нулю в 7-ой бит регистра ODR
        }
    }
}