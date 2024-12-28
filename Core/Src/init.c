#include "init.h"

void GPIO_Ini(void) {
    // 开启 GPIO B, C, D 时钟
    SET_BIT(RCC_GPIO_EN, RCC_GPIOB_EN | RCC_GPIOC_EN | RCC_GPIOD_EN);

    // 初始化板载 LED 引脚
    uint8_t board_leds[] = {BOARD_LED1_PIN, BOARD_LED2_PIN, BOARD_LED3_PIN,CN8_LED4_PIN, CN8_LED5_PIN, CN8_LED6_PIN};
    for (int i = 0; i < 6; i++) {
        SET_BIT(GPIOB_MODER, (1 << (board_leds[i] * 2)));  // 输出模式
        CLEAR_BIT(GPIOB_OTYPER, (1 << board_leds[i]));     // 推挽输出
        SET_BIT(GPIOB_OSPEEDR, (1 << (board_leds[i] * 2))); // 中等速度
        CLEAR_BIT(GPIOB_PUPDR, (3 << (board_leds[i] * 2))); // 无上下拉
    }




    // // 初始化 CN8 LED 引脚
    // uint8_t cn8_leds[] = {CN8_LED4_PIN, CN8_LED5_PIN, CN8_LED6_PIN};
    // for (int i = 0; i < 3; i++) {
    //     SET_BIT(GPIOC_MODER, (1 << (cn8_leds[i] * 2)));  // 输出模式
    //     CLEAR_BIT(GPIOC_OTYPER, (1 << cn8_leds[i]));     // 推挽输出
    //     SET_BIT(GPIOC_OSPEEDR, (1 << (cn8_leds[i] * 2))); // 中等速度
    //     CLEAR_BIT(GPIOC_PUPDR, (3 << (cn8_leds[i] * 2))); // 无上下拉
    // }

    // 初始化 CN8 按钮引脚
    uint8_t buttons[] = {6,BUTTON1_PIN, BUTTON3_PIN};
    for (int i = 0; i < 2; i++) {
        SET_BIT(GPIOC_MODER, (0 << (buttons[i] * 2)));  // 输入模式
        CLEAR_BIT(GPIOC_PUPDR, (3 << (buttons[i] * 2))); // 无上下拉
    }
    // // 按钮2特殊情况在 GPIOD
    // SET_BIT(GPIOD_MODER, (0 << (BUTTON2_PIN * 2)));  // 输入模式
    // CLEAR_BIT(GPIOD_PUPDR, (3 << (BUTTON2_PIN * 2))); // 无上下拉
}
