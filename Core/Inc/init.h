#ifndef INIT_H
#define INIT_H

#include  "D:\AAA_TOOL\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\stm32f4xx.h"
#include  "D:\AAA_TOOL\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\STM32F429xx\stm32f429xx.h"



// 硬编码地址定义
#define GPIOB_BASE_ADDR 0x40020400UL  // GPIOB端口基地址（AHB1总线）
#define GPIOC_BASE_ADDR 0x40020800UL  // GPIOC端口基地址（AHB1总线）
#define RCC_BASE_ADDR   0x40023800UL  // RCC（复位和时钟控制）基地址

// GPIO寄存器偏移量定义
#define MODER_OFFSET    0x00  // GPIO模式寄存器偏移量
#define OTYPER_OFFSET   0x04  // GPIO输出类型寄存器偏移量
#define OSPEEDR_OFFSET  0x08  // GPIO输出速度寄存器偏移量
#define PUPDR_OFFSET    0x0C  // GPIO上拉/下拉寄存器偏移量
#define IDR_OFFSET      0x10  // GPIO输入数据寄存器偏移量
#define BSRR_OFFSET     0x18  // GPIO位设置/复位寄存器偏移量
#define AHB1ENR_OFFSET  0x30  // RCC AHB1外设时钟使能寄存器偏移量

// PB7 LED控制宏
#define PB7_MODER_OUTPUT()     (GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODER7) | GPIO_MODER_MODER7_0)
#define PB7_OTYPER_PUSHPULL()  (GPIOB->OTYPER &= ~GPIO_OTYPER_OT7)
#define PB7_OSPEEDR_HIGH()     (GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~GPIO_OSPEEDR_OSPEED7) | GPIO_OSPEEDR_OSPEED7_1)
#define PB7_PUPDR_NOPULL()     (GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD7)
#define PB7_LED_ON()           (GPIOB->BSRR = GPIO_BSRR_BS_7)
#define PB7_LED_OFF()          (GPIOB->BSRR = GPIO_BSRR_BR_7)

// PB8 LED控制宏
#define PB8_MODER_OUTPUT()     (GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODER8) | GPIO_MODER_MODER8_0)
#define PB8_OTYPER_PUSHPULL()  (GPIOB->OTYPER &= ~GPIO_OTYPER_OT8)
#define PB8_OSPEEDR_HIGH()     (GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~GPIO_OSPEEDR_OSPEED8) | GPIO_OSPEEDR_OSPEED8_1)
#define PB8_PUPDR_NOPULL()     (GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD8)
#define PB8_LED_ON()           (GPIOB->BSRR = GPIO_BSRR_BS_8)
#define PB8_LED_OFF()          (GPIOB->BSRR = GPIO_BSRR_BR_8)

// LED控制函数声明
static inline void led_on(GPIO_TypeDef* port, uint8_t pin)  { port->BSRR = (1U << pin); }
static inline void led_off(GPIO_TypeDef* port, uint8_t pin) { port->BSRR = (1U << (pin + 16U)); }
static inline void pb0_led_on(void)  { *(volatile uint32_t *)(GPIOB_BASE_ADDR + BSRR_OFFSET) = (1U << 0); }
static inline void pb0_led_off(void) { *(volatile uint32_t *)(GPIOB_BASE_ADDR + BSRR_OFFSET) = (1U << (0 + 16U)); }

// PC8模式控制函数声明
void PC8_Set_Input_PullUp(void);    // 设置PC8为输入模式+内部上拉
void PC8_Set_Output_High(void);     // 设置PC8为输出模式+高电平

// 全局变量声明
extern volatile uint8_t g_pc8_mode;  // PC8模式：0=输入模式，1=输出模式

// 按钮标志(用于外部分析)
extern volatile uint8_t button1_flag;  // PC12按钮标志
extern volatile uint8_t button2_flag;  // PC6按钮标志  
extern volatile uint8_t button3_flag;  // PC8按钮标志

// LED状态变量(0:灭，1:亮)
extern volatile uint8_t led_1_state;  // 红色LED状态 (PB14)
extern volatile uint8_t led_2_state;  // 蓝色LED状态 (PB7)
extern volatile uint8_t led_3_state;  // 黄色LED状态 (PB0)
extern volatile uint8_t led_4_state;  // 外部LED状态 (PC8输出时)

// 函数声明
void GPIO_Init_Memory(void);  // GPIO初始化函数

#endif
