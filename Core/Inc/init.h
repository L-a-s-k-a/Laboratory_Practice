#ifndef INIT_H
#define INIT_H

#include  "D:\AAA_TOOL\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\stm32f4xx.h"
#include  "D:\AAA_TOOL\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\STM32F429xx\stm32f429xx.h"

/// 定义引脚号 Определение номеров пинов
#define LED1_PIN 0   // PB0
#define LED2_PIN 7   // PB7
#define LED3_PIN 14  // PB14
#define BUTTON1_PIN 8  // PC8
#define BUTTON2_PIN 13 // PC13

// 函数声明 Объявление функций
void set_PC8_as_output(void);
void set_PC8_as_input(void);
void GPIO_Init_Self_Def(void);

#endif 