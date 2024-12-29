#include "init.h"

void RCC_Ini(void) {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
}

void GPIO_Ini(void) {
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE15_0 | GPIO_MODER_MODE13_0 | GPIO_MODER_MODE12_0 |
                         GPIO_MODER_MODE14_0 | GPIO_MODER_MODE7_0 | GPIO_MODER_MODE0_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT15 | GPIO_OTYPER_OT13 | GPIO_OTYPER_OT12 |
                             GPIO_OTYPER_OT14 | GPIO_OTYPER_OT7 | GPIO_OTYPER_OT0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR15_0 | GPIO_OSPEEDER_OSPEEDR13_0 |
                           GPIO_OSPEEDER_OSPEEDR12_0 | GPIO_OSPEEDER_OSPEEDR14_0 |
                           GPIO_OSPEEDER_OSPEEDR7_0 | GPIO_OSPEEDER_OSPEEDR0_0);

    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD8_1 | GPIO_PUPDR_PUPD9_1 | GPIO_PUPDR_PUPD10_1);
}

void EXTI_ITR_Ini(void) {
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
    MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI8_Msk | SYSCFG_EXTICR3_EXTI9_Msk | SYSCFG_EXTICR3_EXTI10_Msk,
               SYSCFG_EXTICR3_EXTI8_PC | SYSCFG_EXTICR3_EXTI9_PC | SYSCFG_EXTICR3_EXTI10_PC);
    SET_BIT(EXTI->IMR, EXTI_IMR_MR8 | EXTI_IMR_MR9 | EXTI_IMR_MR10);
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR8 | EXTI_RTSR_TR9 | EXTI_RTSR_TR10);
    NVIC_SetPriority(EXTI9_5_IRQn, 2);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void SysTick_Init(void) {
    SysTick->LOAD = 180000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}



// 更新两灯一组的状态
void update_led_pair(uint8_t stage) {
    // 关闭所有灯
    GPIOB->BSRR = (LED1_PIN << 16) | (LED2_PIN << 16) | (LED3_PIN << 16) |
                  (LED4_PIN << 16) | (LED5_PIN << 16) | (LED6_PIN << 16);

    // 点亮对应的两盏灯
    switch (stage) {
        case 0:
            GPIOB->BSRR = LED1_PIN | LED6_PIN; // 点亮 1 和 6
            break;
        case 1:
            GPIOB->BSRR = LED2_PIN | LED5_PIN; // 点亮 2 和 5
            break;
        case 2:
            GPIOB->BSRR = LED3_PIN | LED4_PIN; // 点亮 3 和 4
            break;
    }
}

// 反向关闭两灯一组
void reverse_led_pair(uint8_t stage) {
    switch (stage) {
        case 2:
            GPIOB->BSRR = (LED3_PIN << 16) | (LED4_PIN << 16); // 关闭 3 和 4
            break;
        case 1:
            GPIOB->BSRR = (LED2_PIN << 16) | (LED5_PIN << 16); // 关闭 2 和 5
            break;
        case 0:
            GPIOB->BSRR = (LED1_PIN << 16) | (LED6_PIN << 16); // 关闭 1 和 6
            break;
    }
}

// 翻转单个灯的状态（用于模式2）
void toggle_led(uint8_t led) {
    uint32_t led_pins[] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN, LED5_PIN, LED6_PIN};
    GPIOB->ODR ^= led_pins[led]; // 翻转对应 LED 的状态
}

