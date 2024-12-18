#include "it_handlers.h"

extern uint16_t FLICKER_PERIOD;
extern uint16_t GlobalTickCount,
                ExternInterruptTickCount1,
                ExternInterruptTickCount2, 
                ExternInterruptTickCount3;
extern uint16_t LedState2Data[3][6];
void SysTick_Handler(void)
{
    ExternInterruptTickCount1++;
    ExternInterruptTickCount2++;
    ExternInterruptTickCount3++;
    for(int ii=0;ii<6;ii++){
        LedState2Data[0][ii]++;
    }
    GlobalTickCount++;
}

void EXTI0_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR0);
    b1_clicked();
}

void EXTI9_5_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR9); // Сброс флага прерывания
    b2_clicked();
}

void EXTI15_10_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR12); // Сброс флага прерывания
    b3_clicked();
}