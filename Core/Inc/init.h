#ifndef INIT_H
#define INIT_H

#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"

// RCC 寄存器和时钟使能
#define RCC_AHB1ENR             (*(volatile uint32_t*)0x40023830UL)  // RCC AHB1 peripheral clock enable register
#define RCC_AHB1ENR_GPIOBEN     (1 << 1)  // GPIOB 时钟使能
#define RCC_AHB1ENR_GPIOCEN     (1 << 2)  // GPIOC 时钟使能

// GPIOB 和 GPIOC 寄存器定义
#define GPIOB_MODER             (*(volatile uint32_t*)0x40020400UL)  // GPIOB 模式寄存器
#define GPIOB_BSRR              (*(volatile uint32_t*)0x40020418UL)  // GPIOB 位设置/重置寄存器
#define GPIOC_IDR               (*(volatile uint32_t*)0x40020810UL)  // GPIOC 输入数据寄存器

// GPIOB 引脚定义
#define GPIOB_MODE_PIN0_OUT     (1 << (0 * 2))   // PB0 设置为输出
#define GPIOB_MODE_PIN7_OUT     (1 << (7 * 2))   // PB7 设置为输出
#define GPIOB_MODE_PIN14_OUT    (1 << (14 * 2))  // PB14 设置为输出
#define GPIOB_MODE_PIN15_OUT    (1 << (15 * 2))  // PB15 设置为输出（外接 LED）

// 读取按钮状态
#define READ_GPIO_C13           (GPIOC_IDR & (1 << 13))  // 读取 GPIOC 引脚13（按钮）

// 控制 GPIOB 引脚
#define SET_GPIO_B(pin)         (GPIOB_BSRR = (1 << (pin)))     // 设置 GPIOB 引脚
#define RESET_GPIO_B(pin)       (GPIOB_BSRR = (1 << ((pin) + 16))) // 重置 GPIOB 引脚

// 函数声明
void GPIO_Ini(void);  // GPIO 初始化

#endif // INIT_H