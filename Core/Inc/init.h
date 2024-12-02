#ifndef INIT_H
#define INIT_H
#include  "D:\yangtao-lab_1\Laboratory_Practice\CMSIS\Devices\Inc/stm32f4xx.h"
#include  "D:\yangtao-lab_1\Laboratory_Practice\CMSIS\Devices\Inc/stm32f429xx.h"
// 启用 GPIOB 和 GPIOC 时钟
#define RCC_GPIO_EN                 (*(volatile uint32_t*)0x40023830UL) 
#define RCC_GPIOB_EN                0x02UL 
#define RCC_GPIOC_EN                0x04UL 

// GPIOB 配置寄存器定义
#define GPIOB_MODER                 (*(volatile uint32_t*)0x40020400UL) 
#define GPIOB_OTYPER                (*(volatile uint32_t*)0x40020404UL) 
#define GPIOB_OSPEEDR               (*(volatile uint32_t*)0x40020408UL) 
#define GPIOB_PUPDR                 (*(volatile uint32_t*)0x4002040CUL)  
#define GPIOB_BSRR                  (*(volatile uint32_t*)0x40020418UL) 

// GPIOC 配置寄存器定义（用于按钮）
#define GPIOC_IDR                   (*(volatile uint32_t*)0x40020810UL) 
#define GPIOC_IDR_PIN13             0x002000UL  // 按钮1，PC13
#define GPIOC_IDR_PIN12             0x001000UL  // 按钮2，PC12

// GPIOB 引脚配置（LED1, LED2, LED3, LED4）
#define GPIOB_MODE_PIN0_OUT         0x001UL   // LED1
#define GPIOB_OTYPE_PIN0_PP         0x000000UL 
#define GPIOB_OSPEED_PIN0_MID       0x001UL 
#define GPIOB_PUPDR_PIN0_NOPUPD     0x000000UL
#define GPIOB_BSRR_PIN0_SET         0x001UL 
#define GPIOB_BSRR_PIN0_RESET       0x0010000UL

#define GPIOB_MODE_PIN7_OUT         0x004000UL // LED2
#define GPIOB_OTYPE_PIN7_PP         0x000000UL 
#define GPIOB_OSPEED_PIN7_MID       0x004000UL 
#define GPIOB_PUPDR_PIN7_NOPUPD     0x000000UL 
#define GPIOB_BSRR_PIN7_SET         0x000080UL 
#define GPIOB_BSRR_PIN7_RESET       0x800000UL 

#define GPIOB_MODE_PIN14_OUT        0x0010000000UL // LED3
#define GPIOB_OTYPE_PIN14_PP        0x000000UL 
#define GPIOB_OSPEED_PIN14_MID      0x0010000000UL 
#define GPIOB_PUPDR_PIN14_NOPUPD    0x000000UL
#define GPIOB_BSRR_PIN14_SET        0x004000UL 
#define GPIOB_BSRR_PIN14_RESET      0x0040000000UL

#define GPIOB_MODE_PIN8_OUT         0x0010000UL // LED4
#define GPIOB_OTYPE_PIN8_PP         0x000000UL 
#define GPIOB_OSPEED_PIN8_MID       0x0010000UL 
#define GPIOB_PUPDR_PIN8_NOPUPD     0x000000UL
#define GPIOB_BSRR_PIN8_SET         0x00100UL 

// GPIO 初始化函数
void GPIO_Ini_1(void);

#endif 
// INIT_H
// GPIO 初始化函数，配置 GPIOB 和 GPIOC
void GPIO_Ini_1(void) {
    // 开启 GPIOB 和 GPIOC 时钟
    RCC_GPIO_EN |= (RCC_GPIOB_EN | RCC_GPIOC_EN); 

    // 配置 GPIOB 引脚 0、7、14 为输出模式
    GPIOB_MODER |= (GPIOB_MODE_PIN0_OUT | GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT); 
    GPIOB_OTYPER |= (GPIOB_OTYPE_PIN0_PP | GPIOB_OTYPE_PIN7_PP | GPIOB_OTYPE_PIN14_PP); 
    GPIOB_OSPEEDR |= (GPIOB_OSPEED_PIN0_MID | GPIOB_OSPEED_PIN7_MID | GPIOB_OSPEED_PIN14_MID); 
    GPIOB_PUPDR |= (GPIOB_PUPDR_PIN0_NOPUPD | GPIOB_PUPDR_PIN7_NOPUPD | GPIOB_PUPDR_PIN14_NOPUPD); 
}
