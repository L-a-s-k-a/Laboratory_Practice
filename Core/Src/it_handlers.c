#include "it_handlers.h"

extern uint16_t DelayTickCount;
extern uint16_t LedCount[6];
extern uint32_t GlobalTickBut1Wait;
extern uint8_t flagbut1;
extern uint8_t flagbut1long;
extern uint8_t CurrentState;
extern uint8_t LedCurrfreq[6];
extern uint8_t LedState;

void EXTI9_5_IRQHandler(void)
{
    if(READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0)
    {
        if(flagbut1 == 0){
            flagbut1 = 1;
            GlobalTickBut1Wait = 0;
        }
    }
    else {
        if (flagbut1 == 1 && GlobalTickBut1Wait >= 2000 && GlobalTickBut1Wait < 4000) {
            flagbut1long = 1;
            for (uint8_t i = 0; i < 6; i++) {
                LedCurrfreq[i] = (LedCurrfreq[i] + 1) % 3;
            }
            flagbut1 = 0;
        }
        if (flagbut1 == 1 && GlobalTickBut1Wait >= 4000) {
            if (LedState == 0) {
                LedState = 1;
            } else {
                LedState = 0;
            }
            GlobalTickBut1Wait = 0;
            flagbut1 = 0;
        }
        else if (flagbut1 == 1 && GlobalTickBut1Wait >= 30) {
            flagbut1 = 0;
            CurrentState++;
        }
    }
    SET_BIT(EXTI->PR, EXTI_PR_PR6);
}

void SysTick_Handler(void)
{
    DelayTickCount++;
    if(flagbut1 == 1){
        GlobalTickBut1Wait++;
    }
    for (uint8_t i = 0; i < 6; i++)
    {
        LedCount[i]++;
    }
}

void mydelay(uint32_t delay){
    if(DelayTickCount >= delay) DelayTickCount = 0;
    while(DelayTickCount < delay){}
}
