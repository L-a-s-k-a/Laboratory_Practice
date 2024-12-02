#include "stdint.h"

#include <stdint.h>
#include "init.h"
#include "init.c"

// 函数声明
void GPIO_Ini(void);
void ToggleLED(uint8_t led_index, uint8_t state);
void TurnOffAllLEDs(void);
void TurnOnAllLEDs(void);

 uint8_t button_flag;

int main(void) {
    uint8_t led_index = 0;
    uint8_t button_pressed = 0;

    // 初始化 GPIO
    GPIO_Ini();

    while (1) {
        // 检测按钮状态
        button_flag=!button_pressed;
        if (READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) != 0) { // 按钮按下
            for (volatile int i = 0; i < 100000; i++); // 简单延时用于消抖
            if (READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) == 0 && button_pressed == 0) {
                button_pressed = 1; // 防止重复检测
                led_index++;

                if (led_index >= 1 && led_index <= 4) {
                    // 1~4 次按键：依次点亮 LED，不关闭之前的 LED
                    ToggleLED(led_index, 1);
                } else if (led_index == 5) {
                    // 第 5 次按键：关闭所有 LED
                    TurnOffAllLEDs();
                } else if (led_index == 6) {
                    // 第 6 次按键：点亮所有 LED
                    TurnOnAllLEDs();
                } else if (led_index == 7) {
                    // // 第 7 次按键：重置计数，按第一次逻辑重新开始
                    led_index = 0; // 从第一次逻辑开始（第一个 LED 点亮）
                    TurnOffAllLEDs(); // 点亮第一个 LED
                }
            }
        } else {
            button_pressed = 0; // 按钮释放
        }
    }
}