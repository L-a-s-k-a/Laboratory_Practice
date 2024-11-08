#include "../Inc/init.h"
extern uint8_t LedState; //extern - обозначает, что переменная будет объявленна в другом месте (в файле main.c)
uint8_t BtnCount;
void EXTI15_10_IRQHandler(void)
{
    BtnCount++;

    if (BtnCount >= 2)
    {
        LedState = !LedState;
        BtnCount = 0;
    }
    SET_BIT(EXTI->PR, EXTI_PR_PR13); // выход из прерывания
}