#ifndef __INIT_H
#define __INIT_H

#include "stm32f429xx.h"

// LED引脚定义
#define LED1_PIN    GPIO_ODR_OD0   // PB0 - 绿色（板载）
#define LED2_PIN    GPIO_ODR_OD7   // PB7 - 蓝色（板载）  
#define LED3_PIN    GPIO_ODR_OD14  // PB14 - 红色（板载）
#define LED4_PIN    GPIO_ODR_OD12  // PC12 - 外部LED1
#define LED5_PIN    GPIO_ODR_OD2   // PE2 - 外部LED2
#define LED6_PIN    GPIO_ODR_OD3   // PE3 - 外部LED3

// 按钮引脚定义（按钮1使用PG3）
#define BTN1_PIN    GPIO_IDR_ID3   // PG3 - 按钮1（替换损坏的板载按钮）
#define BTN2_PIN    GPIO_IDR_ID0   // PE0 - 按钮2
#define BTN3_PIN    GPIO_IDR_ID4   // PE4 - 按钮3

// 函数声明
void SystemClock_Config(void);
void GPIO_Init(void);
void LED_On(uint8_t led_num);
void LED_Off(uint8_t led_num);
uint8_t Button_Read(uint8_t btn_num);
void All_LEDs_Off(void);

#endif