#include "../Inc/init.h"
#include "main.h"

extern uint8_t flag1, flag2, flag3, flag4, flag5, flag6, LedState;
extern uint8_t flag1ON, flag2ON, flag3ON, flag4ON, flag5ON, flag6ON;
extern uint8_t BtnCount1, LongBtnCount1, VeryLongBtnCount;
extern uint16_t ledTime1, ledTime2, ledTime3, ledTime4, ledTime5, ledTime6;
extern uint16_t freq1, freq2, freq3, freq4, freq5, freq6;
extern uint16_t GlobalTickCount;
extern uint16_t dtime, cycleTime;
uint16_t led_tick_count;

int freq[3] = {2200, 1500, 300};

int main(void)
{
    ITR_init();
    RCC_Init();
    GPIO_Ini();
    SysTick_Init();

    while (1)
    {

        dtime = GlobalTickCount - cycleTime;
        cycleTime = GlobalTickCount;

        ledTime1 = ledTime1 + dtime;
        if (ledTime1 >= freq[LongBtnCount1])
        {

            flag1ON = !flag1ON;
            ledTime1 = 0;
        }
        if (VeryLongBtnCount == 1)
        {
            if (BtnCount1 == 0)
                
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
            else
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            if (BtnCount1 == 1)
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            else
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            }
            if (BtnCount1 == 2)
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            else
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            }
            if (BtnCount1 == 3)
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15);
                }
            else
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
            }
            if (BtnCount1 == 4)
            {
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6);
                }
            else
            {
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR6);
            }
            if (BtnCount1 == 5){
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS7);
            }
            else
            {
                SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);
            }
        }
        else
        {
            if (flag1ON)
            {
                if (BtnCount1 == 0)
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
                else
                {
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
                }
                if (BtnCount1 == 1)
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                else
                {
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                }
                if (BtnCount1 == 2)
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
                else
                {
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                }
                if (BtnCount1 == 3)
                {
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15);
                    }
                else
                {
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
                }
                if (BtnCount1 == 4)
                {
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6);
                    }
                else
                {
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR6);
                }
                if (BtnCount1 == 5){
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS7);
                }
                else
                {
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);
                }
            }
            if (!flag1ON)
            {
                if (BtnCount1 == 0)
                {
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
                }
                if (BtnCount1 == 1)
                {
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                }
                if (BtnCount1 == 2)
                {
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                }
                if (BtnCount1 == 3)
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
                if (BtnCount1 == 4)
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR6);
                if (BtnCount1 == 5)
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);
            } 
        }
    }
}