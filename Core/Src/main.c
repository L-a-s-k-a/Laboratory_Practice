#include "../Inc/init.h"
#include "../Inc/it_handlers.h"

uint8_t LedState; 

int main(void) 
{ 
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