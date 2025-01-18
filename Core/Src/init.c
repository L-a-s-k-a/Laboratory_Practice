#include "init.h"

void GPIO_Ini(void) {
    // 启用 GPIOB 和 GPIOC 的时钟
    RCC_AHB1ENR |= (RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // 设置 PB0, PB7, PB14, PB15 为输出模式
    GPIOB_MODER &= ~(0x03 << (0 * 2));  // 清除 PB0 的模式
    GPIOB_MODER |= (0x01 << (0 * 2));   // 设置 PB0 为输出

    GPIOB_MODER &= ~(0x03 << (7 * 2));  // 清除 PB7 的模式
    GPIOB_MODER |= (0x01 << (7 * 2));   // 设置 PB7 为输出

    GPIOB_MODER &= ~(0x03 << (14 * 2)); // 清除 PB14 的模式
    GPIOB_MODER |= (0x01 << (14 * 2));  // 设置 PB14 为输出

    GPIOB_MODER &= ~(0x03 << (15 * 2)); // 清除 PB15 的模式
    GPIOB_MODER |= (0x01 << (15 * 2));  // 设置 PB15 为输出
}