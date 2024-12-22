#include "init.h"
#define DELAY_BUTTON_FILTER 500

void EXTI15_10_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void SysTick_Handler(void);


void User_Delay(uint32_t delay);