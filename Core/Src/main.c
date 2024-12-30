#include <stdint.h> 
#include <init.h>

volatile uint8_t led_states[4] = {0, 0, 0, 0}; // 记录每个 LED 的状态
volatile uint8_t button_count = 0;            // 按钮按下次数
volatile uint8_t button_flag = 0;

int main(void) {
    uint8_t button_pressed = 0; // 按钮状态
    GPIO_Ini();                 // 初始化 GPIO

    while (1) {
        // 更新按钮状态
        button_flag = READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) ? 1 : 0;

        // 检测按钮是否按下
        if (READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) != 0) { // 按钮按下
            for (volatile int i = 0; i < 100000; i++); // 简单延时，用于消抖
            if (READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) == 0 && button_pressed == 0) {
                button_pressed = 1; // 标记按钮已经按下
                button_count++;     // 增加按钮按下次数

                // 根据按钮按下次数控制灯
                if (button_count >= 1 && button_count <= 4) {
                    // 点亮指定数量的 LED
                    for (uint8_t i = 0; i < 4; i++) {
                        if (i < button_count) {
                            ToggleLED(i + 1, 1); // 打开 LED
                            led_states[i] = 1;  // 更新状态
                        } else {
                            ToggleLED(i + 1, 0); // 关闭 LED
                            led_states[i] = 0;  // 更新状态
                        }
                    }
                } else if (button_count == 5) {
                    // 关闭所有 LED
                    TurnOffAllLEDs();
                    for (uint8_t i = 0; i < 4; i++) {
                        led_states[i] = 0; // 更新状态
                    }
                } else if (button_count == 6) {
                    // 点亮所有 LED
                    TurnOnAllLEDs();
                    for (uint8_t i = 0; i < 4; i++) {
                        led_states[i] = 1; // 更新状态
                    }
                } else if (button_count > 6) {
                    // 重置逻辑并重新开始
                    TurnOffAllLEDs();
                    for (uint8_t i = 0; i < 4; i++) {
                        led_states[i] = 0; // 更新状态
                    }
                    button_count = 0; // 重置计数
                }
            }
        } else {
            button_pressed = 0; // 按钮释放
        }
    }
}