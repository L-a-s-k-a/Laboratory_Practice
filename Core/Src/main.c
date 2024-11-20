#include "../Inc/init.h"

uint8_t flag1, flag2, flag3, flag4, flag5, flag6, LedState;
uint8_t flag1ON, flag2ON, flag3ON, flag4ON, flag5ON, flag6ON;
uint8_t BtnCount1, LongBtnCount1, BtnCount2, LongBtnCount2;
uint16_t ledTime1, ledTime2, ledTime3, ledTime4, ledTime5, ledTime6;
uint16_t freq1, freq2, freq3, freq4, freq5, freq6;
uint16_t GlobalTickCount;
uint16_t dtime, cycleTime;
int freq[3][3] = {{2500, 769, 500}, {1666, 625, 454}, {1250, 526, 400}};
int main(void)
{
    ITR_init();     // инициализация прерываний
    RCC_Init();     // инициализация таймеров
    GPIO_Ini();     // инициализация портов
    SysTick_Init(); // инициализация системного таймера
    while (1)
    {
        // шаг дискретизации
        dtime = GlobalTickCount - cycleTime;
        cycleTime = GlobalTickCount;
        if (LongBtnCount2 >= 4 || LongBtnCount2 == 0)
        {
            LongBtnCount2 = 1;
        }
        if (LongBtnCount1 >= 4 || LongBtnCount1 == 0)
        {
            LongBtnCount1 = 1;
        }
        // добавить функцию изменения герцовки при долгом нажатии
        switch (BtnCount2)
        {
        case 1:
            freq1 = freq[LongBtnCount2 - 1][LongBtnCount1 - 1];
            break;
        case 2:
            freq2 = freq[LongBtnCount2 - 1][LongBtnCount1 - 1];
            break;
        case 3:
            freq3 = freq[LongBtnCount2 - 1][LongBtnCount1 - 1];
            break;
        case 4:
            freq4 = freq[LongBtnCount2 - 1][LongBtnCount1 - 1];
            break;
        case 5:
            freq5 = freq[LongBtnCount2 - 1][LongBtnCount1 - 1];
            break;
        case 6:
            freq6 = freq[LongBtnCount2 - 1][LongBtnCount1 - 1];
            BtnCount2 = 0;
            break;
        default:
            BtnCount2 = 1;
            break;
        }
        // ------------
        if (flag1)
        {
            ledTime1 = ledTime1 + dtime;
            if (ledTime1 >= freq1)
            {
                flag1ON = !flag1ON;
                ledTime1 = 0;
            }
        }
        else
        {
            ledTime1 = 0;
            flag1ON = 0;
        }
        if (flag2)
        {
            ledTime2 = ledTime2 + dtime;
            if (ledTime2 >= freq2)
            {
                flag2ON = !flag2ON;
                ledTime2 = 0;
            }
        }
        else
        {
            ledTime2 = 0;
            flag2ON = 0;
        }
        if (flag3)
        {
            ledTime3 = ledTime3 + dtime;
            if (ledTime3 >= freq3)
            {
                flag3ON = !flag3ON;
                ledTime3 = 0;
            }
        }
        else
        {
            ledTime3 = 0;
            flag3ON = 0;
        }
        if (flag4)
        {
            ledTime4 = ledTime4 + dtime;
            if (ledTime4 >= freq4)
            {
                flag4ON = !flag4ON;
                ledTime4 = 0;
            }
        }
        else
        {
            ledTime4 = 0;
            flag4ON = 0;
        }
        // ------------
        switch (BtnCount1)
        { // разрешение на мигание светодиодом
        case 1:
            flag1 = 1;
            break;
        case 2:
            flag2 = 1;
            break;
        case 3:
            flag3 = 1;
            break;
        case 4:
            flag4 = 1;
            break;
        case 5:
            flag1 = 0;
            flag2 = 0;
            flag3 = 0;
            flag4 = 0;
            BtnCount1 = 0;
            break;
        default:
            BtnCount1 = 0;
            break;
        }
        if (flag1ON)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13); // Установка единицы в 7-ой бит регистра ODR
        else
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13); // Установка единицы в 7-ой бит регистра ODR
        if (flag2ON)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15); // Установка единицы в 7-ой бит регистра ODR
        else
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15); // Установка единицы в 7-ой бит регистра ODR
        if (flag3ON)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS12); // Установка единицы в 7-ой бит регистра ODR
        else
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR12); // Установка единицы в 7-ой бит регистра ODR
        if (flag4ON)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS6); // Установка единицы в 7-ой бит регистра ODR
        else
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR6); // Установка единицы в 7-ой бит регистра ODR
        //-------------------------------------------------
    }
}