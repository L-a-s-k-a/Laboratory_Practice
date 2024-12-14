#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"
 
// LED引脚定义
#define LED1_PIN        (1 << 0)    // PB0
#define LED2_PIN        (1 << 7)    // PB7
#define LED3_PIN        (1 << 14)   // PB14

// 按钮引脚定义
#define BUTTON1_PIN     (1 << 0)    // PA0
#define BUTTON2_PIN     (1 << 1)    // PA1

// GPIO寄存器地址
#define GPIOB_MODER     (*((volatile uint32_t *)0x40020400)) // GPIOB模式寄存器
#define GPIOB_ODR       (*((volatile uint32_t *)0x40020414)) // GPIOB输出数据寄存器
#define GPIOA_IDR       (*((volatile uint32_t *)0x40020010)) // GPIOA输入数据寄存器

// LED控制宏定义
#define LED1_ON()       (GPIOB_ODR |= LED1_PIN)
#define LED1_OFF()      (GPIOB_ODR &= ~LED1_PIN)
#define LED2_ON()       (GPIOB_ODR |= LED2_PIN)
#define LED2_OFF()      (GPIOB_ODR &= ~LED2_PIN)
#define LED3_ON()       (GPIOB_ODR |= LED3_PIN)
#define LED3_OFF()      (GPIOB_ODR &= ~LED3_PIN) 
 
void GPIO_Ini(void);