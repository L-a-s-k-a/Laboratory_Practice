
#include "../Inc/init.h"

// GPIO初始化1：初始化GPIOB（LED控制引脚）
void GPIO_Ini_1(void) {
    // 使能GPIOB时钟
    RCC_GPIO_EN |= RCC_GPIOB_EN;

    // 配置GPIOB引脚0、7、14、8为输出模式
    GPIOB_MODER |= GPIOB_MODE_PIN0_OUT | GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT | GPIOB_MODE_PIN8_OUT;

    // 配置推挽输出
    GPIOB_OTYPER &= ~(GPIOB_OTYPE_PIN0_PP | GPIOB_OTYPE_PIN7_PP | GPIOB_OTYPE_PIN14_PP | GPIOB_OTYPE_PIN8_PP);

    // 配置中速输出
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN0_MID | GPIOB_OSPEED_PIN7_MID | GPIOB_OSPEED_PIN14_MID | GPIOB_OSPEED_PIN8_MID;

    // 配置无上拉/下拉电阻
    GPIOB_PUPDR &= ~(GPIOB_PUPDR_PIN0_NOPUPD | GPIOB_PUPDR_PIN7_NOPUPD | GPIOB_PUPDR_PIN14_NOPUPD | GPIOB_PUPDR_PIN8_NOPUPD);
}

// GPIO初始化2：初始化GPIOC（按钮输入引脚）
void GPIO_Ini_2(void) {
    // 使能GPIOC时钟
    RCC_GPIO_EN |= RCC_GPIOC_EN;

    // 配置GPIOC引脚12和13为输入模式
    GPIOC_IDR |= GPIOC_IDR_PIN12 | GPIOC_IDR_PIN13;
}

// 点亮LED
void LED_On(uint32_t pin) {
    GPIOB_BSRR |= pin;  // 设置对应的引脚为高电平，点亮LED
}

// 熄灭LED
void LED_Off(uint32_t pin) {
    GPIOB_BSRR |= (pin << 16);  // 设置对应的引脚为低电平，熄灭LED
}
