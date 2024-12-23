#include "it_handlers.h" 

extern uint8_t LedState; //extern - обозначает, что переменная будет объявленна в другом месте (в файле main.c)
extern uint32_t LedCount[6];
uint8_t BtnCount; 


void EXTI15_10_IRQHandler(void){ 
    BtnCount++; 
    if(BtnCount >= 2){ 
        LedState = !LedState; 
        BtnCount = 0; 
    } 
    SET_BIT(EXTI->PR, EXTI_PR_PR13); 
}  

extern uint16_t GlobalTickCount, DelayTickCount; 
uint16_t ExternInterruptTickCount; 
 
void SysTick_Handler(void) 
{ 
    ExternInterruptTickCount++; 
    GlobalTickCount++; 
    DelayTickCount++;
    GlobalTickBut1++;
    GlobalTickBut2++;
    for (uint8_t i = 0; i < 6, i++)
    {
        LedCount[i]++;
    }
} 

void mydelay(uint32_t delay){  
    delay = delay*10000;
    if(DelayTickCount >= delay) DelayTickCount = 0;
    while(DelayTickCount < delay){} //Цикл, благодаря которому происходит задержка программы 
} 