#include "../Inc/init.h"

uint8_t flag1, flag2, flag3, flag4, flag5, flag6, LedState;
uint8_t flag1ON, flag2ON, flag3ON, flag4ON, flag5ON, flag6ON;
uint8_t BtnCount1, LongBtnCount1, VeryLongBtnCount;
uint16_t ledTime1, ledTime2, ledTime3, ledTime4, ledTime5, ledTime6;
uint16_t freq1, freq2, freq3, freq4, freq5, freq6;
uint16_t GlobalTickCount;
uint16_t dtime, cycleTime;
int freq[3] = {2200, 1500, 300};


int main(void){

    ITR_init();     // инициализация прерываний
    RCC_Init();     // инициализация таймеров
    GPIO_Ini();     // инициализация портов
    SysTick_Init(); // инициализация системного таймера


    dtime = GlobalTickCount - cycleTime;
    cycleTime = GlobalTickCount;

    if (LongBtnCount1 >= 4 || LongBtnCount1 == 0)
    {
        LongBtnCount1 = 1;
    }


    ledTime1 = ledTime1 + dtime;
    if (ledTime1 >= freq[LongBtnCount1 - 1])
    {
        flag1ON = !flag1ON;
        ledTime1 = 0;
    }
    if (flag1ON){
        if (BtnCount1 == 1)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0); 
            
        else
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);

        if (BtnCount1 == 2)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); 
            
        else
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);

        if (BtnCount1 == 3)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14); 
            
        else
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
        }

}