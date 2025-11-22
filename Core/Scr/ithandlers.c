#include "../Inc/ithandlers.h"
extern uint16_t GlobalTickCount;
uint16_t ButtonTickCount;
extern uint8_t flag;
uint8_t flag_b = 0;
uint16_t EnterruptCount;
void SysTick_Handler(void)
{
    EnterruptCount++;
    ButtonTickCount++;
    GlobalTickCount++;
}
void EXTI15_10_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR13);
    if (EnterruptCount >= 50)
    {
        EnterruptCount = 0;
        if (flag_b == 0)
        {
            flag_b = 1;
            ButtonTickCount = 0;
        }
        else
        {
            flag_b = 0;
            if (ButtonTickCount < 2000)
            {
                flag = 0;
            }
            else if (ButtonTickCount < 4000 && ButtonTickCount >= 2000)
            {
                flag = 1;
            }
            else if (ButtonTickCount >= 4000)
            {
                flag = 2;
            }
        }
    }
}

