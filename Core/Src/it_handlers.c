#include "it_handlers.h" 
 
extern uint16_t GlobalTickCount; 
uint16_t ExternInterruptTickCount, DelayTickCount; 
 
void SysTick_Handler(void) 
{ 
    ExternInterruptTickCount++; 
    GlobalTickCount++; 
} 
 
extern uint8_t LedState; // extern - обозначает, что переменная будет объявленна в другом месте (в файле main.c) 
uint8_t BtnCount; 
 
void EXTI15_10_IRQHandler(void) 
{ 
SET_BIT(EXTI->PR, EXTI_PR_PR13); //Сброс флага прерывания 
if(ExternInterruptTickCount >= DELAY_BUTTON_FILTER){ //Выполнится, когда пройдёт 100 мс с момента обнуления данной переменной  
BtnCount++; //Изменение состояния кнопки 
ExternInterruptTickCount = 0; //Обнуление счётчика таймера 
} 
} 
void User_Delay(uint32_t delay){  
while(DelayTickCount < delay){} //Цикл, благодаря которому происходит задержка программы 
if(DelayTickCount >= delay) DelayTickCount = 0; //Обнуление переменной счётчика, при достижении заданного пользователем значения 
} 