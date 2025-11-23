/* #include "init.h"
#include "it_handlers.h"

extern uint8_t LedState;
uint8_t BtnCount;
void EXTI15_10_IRQHandler(void)
{
    BtnCount++;
    if(BtnCount >=2)
    {
        LedState = !LedState;
        BtnCount = 0;
    }
    SET_BIT(EXTI->PR, EXTI_PR_PR1);
} */


/* int main(void) // прерывания
{
    // векторы - адреса триггеров по которым иду прерывания
    // внутренние вызываются переферией МК, внешние - внешним миром
    //   
} */