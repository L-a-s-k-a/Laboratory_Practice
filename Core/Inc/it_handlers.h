#include "init.h"
void EXTI15_10_IRQHandler(void);//Прототип обработчика прерываний от линий EXTI 10-15
void SysTick_Handler(void);// Прототип обработчика прерываний системного таймера
void EXTI_ITR_Init(void);//Прототип функции инициализации системы внешних прерываний
void SysTick_Init(void);// Прототип функции инициализации системного таймера