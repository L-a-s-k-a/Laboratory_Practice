#include <stdint.h>
#include "init.h"


// 初始化 GPIO
void GPIO_Ini(void) {
    // 开启 GPIOB 和 GPIOC 时钟
    SET_BIT(*(volatile uint32_t *)RCC_GPIO_EN, RCC_GPIOB_EN | RCC_GPIOC_EN);

    // 配置 GPIOB LED 引脚为输出模式
    *(volatile uint32_t *)GPIOB_MODER |= GPIOB_MODE_PIN0_OUT | GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT | GPIOB_MODE_PIN10_OUT;

    // 配置输出类型为推挽（OTYPER 默认为 0，不需额外设置）
    // 设置输出速度为中速（OSPEEDR 默认为中速，不需额外设置）

    // 配置为无上下拉（PUPDR 默认为无上下拉，不需额外设置）
}

// 切换 LED 状态
void ToggleLED(uint8_t led_index, uint8_t state) {
    switch (led_index) {
        case 1:
            *(volatile uint32_t *)GPIOB_BSRR = state ? GPIOB_BSRR_PIN0_SET : GPIOB_BSRR_PIN0_RESET;
            break;
        case 2:
            *(volatile uint32_t *)GPIOB_BSRR = state ? GPIOB_BSRR_PIN7_SET : GPIOB_BSRR_PIN7_RESET;
            break;
        case 3:
            *(volatile uint32_t *)GPIOB_BSRR = state ? GPIOB_BSRR_PIN14_SET : GPIOB_BSRR_PIN14_RESET;
            break;
        case 4:
            *(volatile uint32_t *)GPIOB_BSRR = state ? GPIOB_BSRR_PIN10_SET : GPIOB_BSRR_PIN10_RESET;
            break;
    }
}

void TurnOnAllLEDs1(uint8_t led_index, uint8_t state) {
    *(volatile uint8_t *)*(volatile uint8_t *)GPIOB_BSRR = state ? GPIOB_BSRR_PIN0_SET : GPIOB_BSRR_PIN0_RESET;
    }

// 关闭所有 LED
void TurnOffAllLEDs(void) {
    *(volatile uint32_t *)GPIOB_BSRR = GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | GPIOB_BSRR_PIN10_RESET;
}

// 点亮所有 LED
void TurnOnAllLEDs(void) {
    *(volatile uint32_t *)GPIOB_BSRR = GPIOB_BSRR_PIN0_SET | GPIOB_BSRR_PIN7_SET | GPIOB_BSRR_PIN14_SET | GPIOB_BSRR_PIN10_SET;
}


