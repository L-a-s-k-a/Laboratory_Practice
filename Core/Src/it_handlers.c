#include "it_handlers.h" 

extern uint16_t DelayTickCount;
extern uint16_t Led_count[6];
extern uint32_t GlobalTickBut1Wait, GlobalTickBut2Wait;
extern uint8_t flagbut1, flagbut2;
extern uint8_t flagbut1long, flagbut2long;
extern uint8_t Current_state;
extern uint8_t Current_led;
extern uint8_t Current_freq[6][2];


void EXTI9_5_IRQHandler(void) 
{
    if(READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0)
    {
        if(flagbut1 == 0){
            flagbut1 = 1;
            GlobalTickBut1Wait = 0;
        }
    }
    else{
        if (flagbut1 == 1 && GlobalTickBut1Wait >= LONG_PRESS_TIME)
        {
            flagbut1long = 1;
            Current_freq[Current_led][0]++;
            
            flagbut1 = 0;
        }
        else if (flagbut1 == 1 && GlobalTickBut1Wait >= SHORT_PRESS_TIME)
        {
            flagbut1 = 0;
            Current_state++;
        }
    }
    SET_BIT(EXTI->PR, EXTI_PR_PR6); 

}
void EXTI15_10_IRQHandler(void){ //but2
    if(READ_BIT(GPIOC->IDR, GPIO_IDR_ID10) != 0)
    {
        if(flagbut2 == 0){
            flagbut2 = 1;
            GlobalTickBut2Wait = 0;
        }
    }
    else{
        if (flagbut2 == 1 && GlobalTickBut2Wait >= LONG_PRESS_TIME)
        {
            flagbut2long = 1;
            Current_freq[Current_led][1]++;
            
            flagbut2 = 0;
        }
        else if (flagbut2 == 1 && GlobalTickBut2Wait >= SHORT_PRESS_TIME)
        {
            flagbut2 = 0;
            Current_led++;
            if (Current_led >= 6)
            {
                Current_led = 0;
            }
        }
    }
    SET_BIT(EXTI->PR, EXTI_PR_PR10); 
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
        Led_count[i]++;
    }
} 

void User_Delay(uint32_t delay){  
    if(DelayTickCount >= delay) DelayTickCount = 0;
    while(DelayTickCount < delay){} //Цикл, благодаря которому происходит задержка программы 
} 