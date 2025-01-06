#include "init.h"

// GPIO 初始化函数1：使能 GPIOB 和 GPIOC 的时钟
void GPIO_Ini_1(void)
{
    // 使能 GPIOB 和 GPIOC 的时钟
    SET_BIT(RCC_GPIO_EN, RCC_GPIOB_EN | RCC_GPIOC_EN);
}

// GPIO 初始化函数2：配置 GPIOB 的 LED 引脚为输出，GPIOC 的按钮引脚为输入
void GPIO_Ini_2(void)
{
    // 配置 GPIOB 的 LED 引脚模式为输出

    // 配置 PB0
    GPIOB_MODER &= ~(0x3 << (0 * 2)); // 清除模式位
    GPIOB_MODER |= GPIOB_MODE_PIN0_OUT;
    GPIOB_OTYPER &= ~(0x1 << 0);      // 推挽输出
    GPIOB_OSPEEDR &= ~(0x3 << (0 * 2)); // 清除速度位
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN0_MID;
    GPIOB_PUPDR &= ~(0x3 << (0 * 2));   // 清除上下拉

    // 配置 PB7
    GPIOB_MODER &= ~(0x3 << (7 * 2));
    GPIOB_MODER |= GPIOB_MODE_PIN7_OUT;
    GPIOB_OTYPER &= ~(0x1 << 7);
    GPIOB_OSPEEDR &= ~(0x3 << (7 * 2));
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN7_MID;
    GPIOB_PUPDR &= ~(0x3 << (7 * 2));

    // 配置 PB14
    GPIOB_MODER &= ~(0x3 << (14 * 2));
    GPIOB_MODER |= GPIOB_MODE_PIN14_OUT;
    GPIOB_OTYPER &= ~(0x1 << 14);
    GPIOB_OSPEEDR &= ~(0x3 << (14 * 2));
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN14_MID;
    GPIOB_PUPDR &= ~(0x3 << (14 * 2));

    // 配置 GPIOC 的按钮引脚为输入，使用上拉电阻

    // 配置 PC6
    GPIOC_MODER &= ~(0x3 << (6 * 2)); // 输入模式
    GPIOC_PUPDR &= ~(0x3 << (6 * 2));
    GPIOC_PUPDR |= (0x01 << (6 * 2)); // 上拉

    // 配置 PC13
    GPIOC_MODER &= ~(0x3 << (13 * 2)); // 输入模式
    GPIOC_PUPDR &= ~(0x3 << (13 * 2));
    GPIOC_PUPDR |= (0x01 << (13 * 2)); // 上拉
}
