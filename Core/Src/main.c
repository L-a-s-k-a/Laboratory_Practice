#include "../Inc/init.h"

uint8_t flag1;

int main(void)
{
    GPIO_Ini(); // инициализация портов
    while (1)
    {
        if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)) // проверка бита
        {
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // Установка единицы в 7-ой бит регистра ODR
            flag1 = 1;
        }
        else
        {
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); // Установка нуля в 7-ой бит регистра ODR
            flag1 = 0;
        }
    }
}