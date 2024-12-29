#ifndef __INIT_H
#define __INIT_H

#include "stm32f4xx.h"  // 这是常见的CMSIS设备头文件，里边会有对GPIOx的结构体定义等

/***** 宏定义：GPIOx 时钟使能位 (以F429为例) *****/
#define RCC_AHB1ENR_GPIOB_EN   (1 << 1)   // RCC_AHB1ENR中使能GPIOB
#define RCC_AHB1ENR_GPIOC_EN   (1 << 2)   // RCC_AHB1ENR中使能GPIOC

/***** 板载RGB LED引脚(PB0/7/14) *****/
#define LED_GREEN_PIN   0   // PB0
#define LED_BLUE_PIN    7   // PB7
#define LED_RED_PIN     14  // PB14

/***** 模式指示LED (PB1, PB2, PB6) *****/
#define LED_MODE1_PIN   1   // PB1
#define LED_MODE2_PIN   2   // PB2
#define LED_MODE3_PIN   6   // PB6

/***** 按键引脚 *****/
#define BTN_MODE_PIN    13  // PC13 (板载，模式切换)
#define BTN1_PIN        8   // PC8  (外接)
#define BTN2_PIN        9   // PC9  (外接)
#define BTN3_PIN        10  // PC10 (外接)

/***** 函数声明 *****/
void GPIO_Init(void);
void delay_ms(volatile uint32_t ms);

#endif
