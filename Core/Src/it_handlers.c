#include "it_handlers.h"
#include "main.h"
#include "init.h"

volatile uint8_t flag1, flag2, flag3, flag4, flag5, flag6, LedState;
volatile uint8_t flag1ON, flag2ON, flag3ON, flag4ON, flag5ON, flag6ON;
volatile uint8_t BtnCount1, LongBtnCount1, VeryLongBtnCount;
volatile uint16_t ledTime1, ledTime2, ledTime3, ledTime4, ledTime5, ledTime6;
volatile uint16_t freq1, freq2, freq3, freq4, freq5, freq6;
volatile uint16_t GlobalTickCount;
volatile uint16_t dtime, cycleTime;
uint16_t delta_time_press;
int ExternInterruptTickCount, ExternInterruptTickCount2, dTime1, front1, DelayTickCount;


void SysTick_Handler(void)
{
    ExternInterruptTickCount++;
    ExternInterruptTickCount2++;
    GlobalTickCount++;
}
void EXTI4_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR4);
    if (ExternInterruptTickCount >= DELAY_BUTTON_FILTER)
    {        front1 = !front1;
        if (front1 == 1)
            dTime1 = GlobalTickCount;
        else
        {
            delta_time_press = GlobalTickCount - (dTime1 - 1);
            if ((GlobalTickCount - (dTime1 - 1) >= 2001) && (GlobalTickCount - (dTime1 - 1) <= 4001))
                LongBtnCount1++;
                if (LongBtnCount1 >=3 )
                {
                    LongBtnCount1 = 0;
                }
            else{
                if (GlobalTickCount - (dTime1 - 1) >= 4001)
                    VeryLongBtnCount = !VeryLongBtnCount;
                else
                {
                    BtnCount1++;
                    if (BtnCount1>=6)
                    {
                       BtnCount1 = 0; 
                    }
                } 
        }
        ExternInterruptTickCount = 0;
    }
}

}
void User_Delay(uint32_t delay)
{
    while (DelayTickCount < delay)
    {
    }
    if (DelayTickCount >= delay)
        DelayTickCount = 0;
}