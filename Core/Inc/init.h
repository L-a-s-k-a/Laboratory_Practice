

#ifndef __INIT_H
#define __INIT_H

#include "stm32f4xx.h" // 或根据你的CMSIS包路径调整

// 全局函数声明
void SystemClock_Config(void);
void GPIO_Init_All(void);
void EXTI_Init_All(void);
void SysTick_Init(uint32_t ticks);

// 简单延时函数 (可选) 
void User_Delay(volatile uint32_t ms);

#endif
