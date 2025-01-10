#include "init.h"
#define DELAY_BUTTON_FILTER 500  // 按钮去抖延迟（单位：毫秒）
// 系统定时器中断处理函数声明
void SysTick_Handler(void);
// 外部中断处理函数声明
void EXTI0_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
