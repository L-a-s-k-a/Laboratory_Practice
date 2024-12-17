#include "init.h" 

void GPIO_Ini(void)
{ 
    // 启用GPIO时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // 启用GPIOB时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // 启用GPIOA时钟

    // 设置PB0, PB7, PB14, PB15为输出模式（01）
    GPIOB_MODER &= ~(3 << (0 * 2));   // 清除PB0模式位
    GPIOB_MODER |= (1 << (0 * 2));    // 设置PB0为输出模式

    GPIOB_MODER &= ~(3 << (7 * 2));   // 清除PB7模式位
    GPIOB_MODER |= (1 << (7 * 2));    // 设置PB7为输出模式

    GPIOB_MODER &= ~(3 << (14 * 2));  // 清除PB14模式位
    GPIOB_MODER |= (1 << (14 * 2));   // 设置PB14为输出模式

    GPIOB_MODER &= ~(3 << (15 * 2));  // 清除PB15模式位
    GPIOB_MODER |= (1 << (15 * 2));   // 设置PB15为输出模式

    // 设置PA0为输入模式（00）
    GPIOA->MODER &= ~(3 << (0 * 2));   // PA0为输入模式
    
    // 配置PA0为上拉输入
    GPIOA->PUPDR |= (1 << (0 * 2));     // PA0上拉
} 