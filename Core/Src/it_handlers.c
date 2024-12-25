#include "it_handlers.h" 

extern uint16_t DelayTickCount;
extern uint16_t LedCount[6];
extern uint32_t GlobalTickBut1Wait, GlobalTickBut2Wait;
extern uint8_t flagbut1, flagbut2;
extern uint8_t flagbut1long, flagbut2long;
extern uint8_t CurrentState;
extern uint8_t CurrentLed;
extern uint8_t counterbut1;
extern uint8_t counterbut2;

void EXTI9_5_IRQHandler(void) //but1
{
    if(READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0)
    {
        if(flagbut1 == 0){
            flagbut1 = 1;
            GlobalTickBut1Wait = 0;
        }
    }
    else{
        if (flagbut1 == 1 && GlobalTickBut1Wait >= 2000)
        {
            flagbut1long = 1;
            counterbut1++;
            flagbut1 = 0;
        }
        else if (flagbut1 == 1 && GlobalTickBut1Wait >= 30)
        {
            flagbut1 = 0;
            CurrentState++;
        }
    }
    SET_BIT(EXTI->PR, EXTI_PR_PR6); 

}
void EXTI15_10_IRQHandler(void){ //but2
    if(READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) != 0)
    {
        if(flagbut2 == 0){
            flagbut2 = 1;
            GlobalTickBut2Wait = 0;
        }
    }
    else{
        if (flagbut2 == 1 && GlobalTickBut2Wait >= 2000)
        {
            flagbut2long = 1;
            counterbut2++;
            flagbut2 = 0;
        }
        else if (flagbut2 == 1 && GlobalTickBut2Wait >= 30)
        {
            flagbut2 = 0;
            CurrentLed++;
            if (CurrentLed >= 6)
            {
                CurrentLed = 0;
            }
        }
    }
    SET_BIT(EXTI->PR, EXTI_PR_PR13); 
}
 
void SysTick_Handler(void) 
{  
    DelayTickCount++;
    if(flagbut1 == 1){
        GlobalTickBut1Wait++;
    }
    if(flagbut2 == 1){
        GlobalTickBut2Wait++;
    }
    for (uint8_t i = 0; i < 6; i++)
    {
        LedCount[i]++;
    }
} 

void mydelay(uint32_t delay){  
    if(DelayTickCount >= delay) DelayTickCount = 0;
    while(DelayTickCount < delay){} //Цикл, благодаря которому происходит задержка программы 
} 