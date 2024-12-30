
#ifndef IT_HANDLERS_H
#define IT_HANDLERS_H

#include "init.h"

// 消抖阈值 (ms)
#define DELAY_BUTTON_FILTER 200

// 声明中断服务函数
void SysTick_Handler(void);
void EXTI9_5_IRQHandler(void);   // 用于PC6/PC7
void EXTI15_10_IRQHandler(void); // 用于PC13

#endif // IT_HANDLERS_H
