#ifndef __INIT_H
#define __INIT_H

#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

// 宏定义：用于翻转寄存器的指定位
#define TOGGLE_BIT(REG, BIT) ((REG) ^= (BIT))

// 函数声明
void RCC_Ini(void);       // 系统时钟初始化函数
void GPIO_Ini(void);      // GPIO初始化函数
void ITR_Ini(void);       // 外部中断初始化函数
void SysTick_Init(void);  // 系统定时器初始化函数

#endif // __INIT_H
