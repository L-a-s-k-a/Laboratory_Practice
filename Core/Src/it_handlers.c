#include "it_handlers.h" 

extern uint8_t LedState; //extern - обозначает, что переменная будет объявленна в другом месте (в файле main.c)
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
} 

void User_Delay(uint32_t delay){  
    DelayTickCount = 0;
    while(DelayTickCount < delay){} //Цикл, благодаря которому происходит задержка программы 
    //if(DelayTickCount >= delay) DelayTickCount = 0; //Обнуление переменной счётчика, при достижении заданного пользователем значения 
} 