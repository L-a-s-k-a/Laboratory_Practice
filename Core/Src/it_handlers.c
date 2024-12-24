#include "it_handlers.h" 

extern uint8_t LedState; //extern - обозначает, что переменная будет объявленна в другом месте (в файле main.c)
extern uint32_t LedCount[6];
extern uint32_t GlobalTickBut1, GlobalTickBut2;
uint32_t GlobalTickBut1Wait = 0, GlobalTickBut2Wait = 0;
uint8_t flagbut1, flagbut2;
uint8_t BtnCount; 


void EXTI9_5_IRQHandler(void) //but1
{
    //через маску можно определить тип фронта
    if(flagbut1 == 0){
        GlobalTickBut1Wait = 0;
    }
    else if (flagbut1 == 0 && GlobalTickBut1Wait >= 30*10000)
    {
         flagbut1 = 0;
    }
    /*
            
                GlobalTickBut2 = 0;    ///////////////////////////////////
                flagbut2 = 1;
                mydelay(20);
            
            if(GlobalTickBut2 >= WAIT_TIME){ //Если прошло 2 секунды 
                counterbut2++;
            } 
            else{ 
                ////////////////////////////////////////////////// кратковременное
            }      
    
    else if(flagbut2 = 1){
            flagbut2 = 0;
    }
    */
    SET_BIT(EXTI->PR, EXTI_PR_PR6); 

}

void EXTI15_10_IRQHandler(void){ //but2
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
    GlobalTickCount++; 

    DelayTickCount++;
    GlobalTickBut1++;
    GlobalTickBut2++;
    if(flagbut1 == 1){
        GlobalTickBut1Wait++;
    }
    if(flagbut2 == 1){
        GlobalTickBut2Wait++;
    }
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