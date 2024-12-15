#include <stdint.h>
#include "init.h"


// 全局变量
volatile uint8_t button_flag = 0;        // 按钮状态
volatile uint8_t led_states[4] = {0};    // 4 个 LED 的状态
volatile uint8_t button_count = 0;       // 按钮按下次数

// 初始化 GPIO
void GPIO_Init(void) {
    // 开启 GPIOB 和 GPIOC 时钟
    SET_BIT(*(volatile uint32_t *)RCC_GPIO_EN, RCC_GPIOB_EN | RCC_GPIOC_EN);

    // 设置 GPIOB 的 PIN0、PIN7、PIN10、PIN14 为输出模式
    *(volatile uint32_t *)GPIOB_MODER |= GPIOB_MODE_PIN0_OUT | GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT | GPIOB_MODE_PIN10_OUT;
}

// 更新 LED 状态
void UpdateLEDs(uint8_t count) {
    // 关闭所有 LED
    *(volatile uint32_t *)GPIOB_BSRR = GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | GPIOB_BSRR_PIN10_RESET;

    // 根据按钮次数更新 LED 状态
    for (uint8_t i = 0; i < 4; i++) {
        led_states[i] = (i < count) ? 1 : 0; // 更新全局变量
        if (led_states[i]) {
            switch (i) {
                case 0: *(volatile uint32_t *)GPIOB_BSRR |= GPIOB_BSRR_PIN0_SET; break;
                case 1: *(volatile uint32_t *)GPIOB_BSRR |= GPIOB_BSRR_PIN7_SET; break;
                case 2: *(volatile uint32_t *)GPIOB_BSRR |= GPIOB_BSRR_PIN14_SET; break;
                case 3: *(volatile uint32_t *)GPIOB_BSRR |= GPIOB_BSRR_PIN10_SET; break;
            }
        }
    }
}
