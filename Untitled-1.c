#include "init.h"
void RCC_Init(void){
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);         //устанавливает значение, которое будет прибавляться к битам, регулирует отклонение частоты
    CLEAR_REG(RCC->CFGR);                               //очистка регистров
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON); 
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON); 
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); 

}