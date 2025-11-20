#ifndef INIT_H
#define INIT_H

#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h"


// 板载LED引脚定义（PB0, PB7, PB14）
#define LED1_PIN    (1 << 0)   // PB0 - 绿色LED
#define LED2_PIN    (1 << 7)   // PB7 - 蓝色LED  
#define LED3_PIN    (1 << 14)  // PB14 - 红色LED
#define LED_PORT    GPIOB

// 外接LED引脚定义（PC10, PC11, PC12）
#define EXT_LED1_PIN    (1 << 10)   // PC10 - 外接LED1
#define EXT_LED2_PIN    (1 << 11)   // PC11 - 外接LED2
#define EXT_LED3_PIN    (1 << 12)   // PC12 - 外接LED3
#define EXT_LED_PORT    GPIOC

// 按钮引脚定义
#define BTN1_PIN    (1 << 3)   // PG3 - 第一个按钮（设置数量）
#define BTN1_PORT   GPIOG
#define BTN2_PIN    (1 << 0)   // PA0 - 第二个按钮（执行）
#define BTN2_PORT   GPIOA

// GPIO模式定义
#define GPIO_MODE_INPUT  0x00
#define GPIO_MODE_OUTPUT 0x01
#define GPIO_MODE_AF     0x02
#define GPIO_MODE_ANALOG 0x03

// 上拉/下拉定义
#define GPIO_NO_PULL     0x00
#define GPIO_PULLUP      0x01
#define GPIO_PULLDOWN    0x02

// LED2控制宏命令
#define LED2_ON()       (LED_PORT->BSRR = LED2_PIN)                    // LED2开启
#define LED2_OFF()      (LED_PORT->BSRR = (LED2_PIN << 16))           // LED2关闭
#define LED2_TOGGLE()   (LED_PORT->ODR ^= LED2_PIN)                   // LED2翻转
#define LED2_SET(state) do { \
    if(state) LED_PORT->BSRR = LED2_PIN; \
    else LED_PORT->BSRR = (LED2_PIN << 16); \
} while(0)                                                           // LED2设置状态

// 函数声明
void SystemClock_Config(void);
void GPIO_Init(void);
uint8_t Read_BTN1(void);
uint8_t Read_BTN2(void);
void LED_Write(uint8_t leds);
void Ext_LED_Write(uint8_t leds);

#endif 