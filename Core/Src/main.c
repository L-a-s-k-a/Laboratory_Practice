#include "stdint.h"
#include "init.h"
#include "init.c"


int main(void) {
    GPIO_Init();
    uint8_t button_pressed = 0;

    while (1) {
        // 检测按钮状态
        button_flag = READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) ? 1 : 0;

        if (button_flag) { // 按钮按下
            for (volatile int i = 0; i < 100000; i++); // 简单延时去抖
            if (!READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) && !button_pressed) {
                button_pressed = 1; // 防止重复检测
                button_count++;

                if (button_count <= 4) {
                    // 1~4 次按键：依次点亮 LED
                    UpdateLEDs(button_count);
                } else if (button_count == 5) {
                    // 第 5 次按键：关闭所有 LED
                    UpdateLEDs(0);
                } else if (button_count == 6) {
                    // 第 6 次按键：全部点亮
                    UpdateLEDs(4);
                } else if (button_count >= 7) {
                    // 第 7 次按键：重置计数和状态
                    UpdateLEDs(0);
                    button_count = 0;
                       }
                    }
                  }
                 }
             }
