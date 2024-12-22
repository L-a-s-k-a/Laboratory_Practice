#include "init.h"

void GPIO_Ini(void) {
    RCC_GPIO_EN |= RCC_GPIOB_EN + RCC_GPIOC_EN; // 启用 GPIOB 和 GPIOC 的时钟

    GPIOB_MODER |= GPIOB_MODE_PIN0_OUT; // 设置 PB0 为输出
    GPIOB_MODER |= GPIOB_MODE_PIN7_OUT; // 设置 PB7 为输出
    GPIOB_MODER |= GPIOB_MODE_PIN14_OUT; // 设置 PB14 为输出
    GPIOB_MODER |= GPIOB_MODE_PIN15_OUT; // 设置 PB15 为输出（外接LED）
}