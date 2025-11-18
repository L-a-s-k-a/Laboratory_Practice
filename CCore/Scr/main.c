#include "../Inc/init.h"
#include "../Inc/interrupt.h"

uint16_t ButtonTickCount = 0;
bool LedMode = false;
uint8_t Frequency = 0;


int main(void)
{
    GPIO_Init();    // Инициализация портов
    RCC_Init();     // Настройка тактирования
    IQR_Init();     // Настройка прерывания на кнопку
    SysTick_Init(); // Настройка таймера

    while (1)
    {
        /*Первый режим мерцания светодиодов*/
        if (LedMode == false)
        {
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12 | GPIO_BSRR_BR11 | GPIO_BSRR_BR10 |
                                     GPIO_BSRR_BR8 | GPIO_BSRR_BR6 | GPIO_BSRR_BR5);
            Mode_Flicker_First(Frequency);
        }
        /*Второй режим мерцания светодиодов*/
        else if (LedMode == true)
        {
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12 | GPIO_BSRR_BR11 | GPIO_BSRR_BR10 |
                                     GPIO_BSRR_BR8 | GPIO_BSRR_BR6 | GPIO_BSRR_BR5);
            Mode_Flicker_Second(Frequency);                   
        }
    }
    return 0;
}

