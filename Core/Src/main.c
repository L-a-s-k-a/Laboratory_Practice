#include "main.h"

int led_state = 0;       // 当前点亮的 LED 数
int button1_pressed = 0; // 按钮 1 状态
int button2_pressed = 0; // 按钮 2 状态
int button3_pressed = 0; // 按钮 3 状态
int swap_flag = 0;       // 按钮 1 和按钮 2 功能交换标志

void GPIO_Init(void) {
    // 1. 使能 GPIO 时钟
    RCC_AHB1ENR |= RCC_GPIOA_EN | RCC_GPIOB_EN | RCC_GPIOC_EN | RCC_GPIOF_EN;

    // 2. 配置 LED 引脚为输出模式
    GPIOB_MODER |= (1 << (0 * 2)) | (1 << (7 * 2)) | (1 << (14 * 2));  // PB0, PB7, PB14
    GPIOA_MODER |= (1 << (3 * 2));                                    // PA3
    GPIOC_MODER |= (1 << (0 * 2)) | (1 << (3 * 2));                   // PC0, PC3

    // 3. 配置按钮引脚为输入模式，开启上拉电阻
    GPIOF_PUPDR |= (1 << (3 * 2)) | (1 << (5 * 2));  // PF3, PF5
}

void Delay(volatile uint32_t time) {
    while (time--);
}

void LED_Control(int state, int on) {
    if (state == 0) { if (on) LED1_ON; else LED1_OFF; }
    else if (state == 1) { if (on) LED2_ON; else LED2_OFF; }
    else if (state == 2) { if (on) LED3_ON; else LED3_OFF; }
    else if (state == 3) { if (on) EXT_LED1_ON; else EXT_LED1_OFF; }
    else if (state == 4) { if (on) EXT_LED2_ON; else EXT_LED2_OFF; }
    else if (state == 5) { if (on) EXT_LED3_ON; else EXT_LED3_OFF; }
}

int main(void) {
    GPIO_Init();

    while (1) {
        // 按钮 1：依次点亮 LED
        if (BUTTON1_PRESSED) {
            if (!button1_pressed) {
                button1_pressed = 1;
                if (led_state < 6) LED_Control(led_state++, 1);
            }
        } else button1_pressed = 0;

        // 按钮 2：依次熄灭 LED
        if (BUTTON2_PRESSED) {
            if (!button2_pressed) {
                button2_pressed = 1;
                if (led_state > 0) LED_Control(--led_state, 0);
            }
        } else button2_pressed = 0;

        // 按钮 3：交换按钮 1 和按钮 2 的功能
        if (BUTTON3_PRESSED) {
            if (!button3_pressed) {
                button3_pressed = 1;
                swap_flag = !swap_flag;
            }
        } else button3_pressed = 0;

        Delay(100000);
    }
}
