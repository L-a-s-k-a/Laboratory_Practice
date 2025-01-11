// it_handlers.h
#ifndef IT_HANDLERS_H
#define IT_HANDLERS_H

#include "init.h"
#include "globals.h"

// 按钮防抖延迟（毫秒）
#define DELAY_BUTTON_FILTER 500

// 中断处理函数声明
void SysTick_Handler(void);
void EXTI0_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

#endif // IT_HANDLERS_H
