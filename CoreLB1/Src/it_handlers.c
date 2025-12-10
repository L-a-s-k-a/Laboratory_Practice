#include "C:\Users\parni\Git_repos\Laboratory_Practice\CoreLB1\Inc\init2.h"

int8_t BtnCount; 
void EXTI15_10_IRQHandler(void)
{ 
    BtnCount++; 
    SET_BIT(EXTI->PR, EXTI_PR_PR13); 
} 