#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"

// --- 对外声明初始化函数 ---
void RCC_Ini(void);
void GPIO_Ini(void);
void EXTI_ITR_Ini(void);
void SysTick_Init(void);

// --- LED 引脚定义（与示例类似） ---
// 注意：BSRR 寄存器写 1 时高16位表示清零，低16位表示置位。
// 如果用 ODR 方式控制，也可以用相应位号定义
#define LED1_PIN    GPIO_BSRR_BS14   // 这里示例把 PB14 称为 LED1
#define LED2_PIN    GPIO_BSRR_BS7
#define LED3_PIN    GPIO_BSRR_BS0
#define LED4_PIN    GPIO_BSRR_BS2
#define LED5_PIN    GPIO_BSRR_BS6
#define LED6_PIN    GPIO_BSRR_BS1

// --- 如需在模式2中按 ODR 方式翻转，可以另外定义 ODR 对应位 ---
// #define LED1_ODR  GPIO_ODR_OD14
// ...

// --- 全局辅助函数，可在 it_handlers.c 或 main.c 中实现 ---
void update_leds_mode1(void);  // 用于模式1下，对灯切换
void update_leds_mode2(void);  // 用于模式2下，多灯独立闪烁

#endif // INIT_H