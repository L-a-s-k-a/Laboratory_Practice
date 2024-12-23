#include <init.h> 
#include <it_handlers.h> 
 
#define FLICKER_PERIOD 2000 //Период мерцания светодиода 
#define FLICKER_SEMIPERIOD 1000 //Период мерцания светодиода 
 
uint16_t GlobalTickCount; 
uint8_t LedState; 
 
int main(void) 
{ 
   GPIO_Ini(); //Инициализация поротв GPIO 
   RCC_Ini(); //Инициализация системы тактирования RCC 
   ITR_Ini(); //Инициализация контроллера прерываний 
   SysTick_Init(); //Инициализация системного таймера 
    
   while (1) 
   { 
        if(LedState) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Если нажали один раз на кнопку светодиод включится 
        else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Если нажали ещё раз светодиод выключится 
 
        if(GlobalTickCount >= FLICKER_PERIOD / 2){ //Если прошло 1000 прерываний системного таймера = 1 секунда 
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Включаем светодиод 
            if(GlobalTickCount >= FLICKER_PERIOD){ //Если прошло 2000 прерываний системного таймера = 2 секунды 
                GlobalTickCount = 0; //Обнуляем переменную счётчика системного таймера 
            } 
        } 
        else{ 
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Иначе выключаем светодиод 
        } 
 
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Включаем светодиод 
        User_Delay(FLICKER_SEMIPERIOD); //Ожидаем 1 секунду 
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Выключаем светодиод 
        User_Delay(FLICKER_SEMIPERIOD); //Ожидаем 1 секунду 
   } 
} 




/*
#include "../Inc/init.h"
#include "../Inc/it_handlers.h"

uint8_t LedState; 

int main(void) 
{ 
    SystemInit()
    GPIO_Ini_Self();
    GPIO_Ini_CMSIS();
    RCC_Init(); // Инициализация тактирования системы 
    ITR_init();// Инициализация прерываний 
    while (1) 
    { 
        if(LedState) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); 
        else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); 
    } 
} 
*/