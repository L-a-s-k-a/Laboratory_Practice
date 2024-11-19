#include "../Inc/init.h"

uint8_t flag1, flag1_pred, LedState, BtnCount1, LongBtnCount1;
uint16_t GlobalTickCount;
int button_count = 0, button_drebezg, button;
int main(void)
{
    ITR_init();     // инициализация прерываний
    RCC_Init();     // инициализация таймеров
    GPIO_Ini();     // инициализация портов
    SysTick_Init(); // инициализация системного таймера
    while (1)
    {

        //-------------------------------------------------
        // lab1
        switch (BtnCount1)
        {
        case 1:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 2:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 3:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS12); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 4:
            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN6_SET); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 5:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR12);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR6);
            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN6_RESET);
            break;
        default:
            button_count = 0;
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR12);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR6);
            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN6_RESET);
            break;
        }
    }
}