#ifndef INIT_H
#define INIT_H

#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h"

//硬编码地址定义真正的直接内存访问

#define GPIOB_BASE_ADDR 0x40020400UL  // GPIOB端口基地址（AHB1总线）
#define GPIOC_BASE_ADDR 0x40020800UL  // GPIOC端口基地址（AHB1总线）
#define GPIOD_BASE_ADDR 0x40020C00UL  // GPIOD端口基地址（AHB1总线）
#define GPIOG_BASE_ADDR 0x40021800UL  // GPIOG端口基地址（AHB1总线）
#define RCC_BASE_ADDR   0x40023800UL  // RCC（复位和时钟控制）基地址

//GPIO寄存器偏移量定义

#define MODER_OFFSET    0x00  // GPIO模式寄存器偏移量（设置输入/输出模式）
#define OTYPER_OFFSET   0x04  // GPIO输出类型寄存器偏移量（推挽/开漏）
#define OSPEEDR_OFFSET  0x08  // GPIO输出速度寄存器偏移量（速度设置）
#define PUPDR_OFFSET    0x0C  // GPIO上拉/下拉寄存器偏移量（上下拉电阻配置）
#define IDR_OFFSET      0x10  // GPIO输入数据寄存器偏移量（读取引脚状态）
#define BSRR_OFFSET     0x18  // GPIO位设置/复位寄存器偏移量（控制输出电平）
#define AHB1ENR_OFFSET  0x30  // RCC AHB1外设时钟使能寄存器偏移量

// PB7配置宏 - 自定义指令和宏

// 设置PB7为输出模式（清除原模式位，设置为01-输出模式）
#define PB7_MODER_OUTPUT()     (GPIOB->MODER = (GPIOB->MODER & ~GPIO_MODER_MODER7) | GPIO_MODER_MODER7_0)

// 设置PB7为推挽输出（清除输出类型位，设置为0-推挽输出）
#define PB7_OTYPER_PUSHPULL()  (GPIOB->OTYPER &= ~GPIO_OTYPER_OT7)

// 设置PB7为高速模式（清除原速度位，设置为10-高速模式）
#define PB7_OSPEEDR_HIGH()     (GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~GPIO_OSPEEDR_OSPEED7) | GPIO_OSPEEDR_OSPEED7_1)

// 设置PB7无上拉下拉电阻（清除上下拉配置位）
#define PB7_PUPDR_NOPULL()     (GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD7)

// PB7 LED控制宏
// 点亮PB7连接的LED（设置BSRR寄存器的BS7位）
#define PB7_LED_ON()           (GPIOB->BSRR = GPIO_BSRR_BS_7)

// 熄灭PB7连接的LED（设置BSRR寄存器的BR7位）
#define PB7_LED_OFF()          (GPIOB->BSRR = GPIO_BSRR_BR_7)

// 全局变量声明
extern volatile uint8_t g_pd4_button_state;       // PD4按键状态
extern volatile uint32_t g_pd4_raw_value;         // PD4原始引脚值
extern volatile uint8_t g_pd4_pressed_flag;       // PD4按下标志
extern volatile uint32_t g_button_debug_counter;  // 按键调试计数器
extern volatile uint8_t g_led_step;               // LED点亮步骤: 0=全灭, 1=PB14, 2=PB7, 3=PB0
extern volatile uint8_t g_pg3_button_state;       // PG3按键状态
extern volatile uint8_t g_pg3_pressed_flag;       // PG3按下标志
extern volatile uint8_t g_system_mode;            // 系统模式: 0=正常模式, 1=PD4输出模式
extern volatile uint8_t g_pd4_mode_state;         // PD4引脚模式状态: 0=输入上拉, 1=输出高电平, 2=无上下拉

// 函数声明
void GPIO_Init_Memory(void);  // GPIO初始化函数 - 使用直接内存访问方式配置GPIO
void PD4_Set_Input_PullUp(void);     // 设置PD4为输入模式+上拉（正常模式）
void PD4_Set_Output_High(void);      // 设置PD4为输出模式+高电平

#endif