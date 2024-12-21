#include <stdint.h>
#include "init.h"
#include "init.c"
/* // 定义 LED 状态变量
uint8_t led_state = 0; // LED 状态
uint8_t button1_state = 0; // 按钮1状态
uint8_t button2_state = 0; // 按钮2状态
uint8_t mode = 0; // 模式，0：开启 LED，1：关闭 LED
uint8_t button1_flag;
uint8_t button2_flag;
#define DEBOUNCE_DELAY 200 // 防抖延迟，单位为毫秒

// 延时函数
void delay(uint32_t s) {
    while (s--) {
        // 添加 CPU 休眠或者空循环
        __NOP();
    }
}

// 主程序
int main(void) {
    GPIO_Ini_1(); // 初始化 GPIO 引脚

    while (1) {
        // 按钮1检测（PC12）
         button1_flag =! button1_state;
         button2_flag =! button2_state;
        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) == 0) && (button1_state == 0)) { // 按钮按下
            delay(DEBOUNCE_DELAY); // 防抖延时
            if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) == 0) { // 按钮仍被按下
                mode ^= 1; // 切换模式
                button1_state = 1; // 更新按钮状态
            }
        } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) { // 按钮松开
            button1_state = 0; // 重置按钮1状态
        }

        // 按钮2检测（PC13）
        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) && (button2_state == 0)) { // 按钮按下
            delay(DEBOUNCE_DELAY); // 防抖延时
            if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) { // 按钮仍被按下
                if (mode == 0) { // 开启模式
                    if (led_state < 3) {
                        switch (led_state) {
                            case 0:
                                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); // 点亮 LED1
                                break;
                            case 1:
                                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); // 点亮 LED2
                                break;
                            case 2:
                                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET); // 点亮 LED3
                                break;
                        }
                        led_state++; // 更新状态
                    }
                } else { // 关闭模式
                    if (led_state > 0) {
                        switch (led_state) {
                            case 1:
                                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET)); // 熄灭 LED1
                                break;
                            case 2:
                                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET)); // 熄灭 LED2
                                break;
                            case 3:
                                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_RESET)); // 熄灭 LED3
                                break;
                        }
                        led_state--; // 更新状态
                    }
                }

                button2_state = 1; // 更新按钮2状态
            }
        } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0) { // 按钮松开
            button2_state = 0; // 重置按钮2状态
        }

        delay(20000); // 防止程序过快循环
    }
} */
// LED 状态管理变量
uint8_t led_index = 0; // LED 当前状态
uint8_t button1_active = 0; // 按钮1状态
uint8_t button2_active = 0; // 按钮2状态
uint8_t current_mode = 0; // 当前模式，0: 打开 LED，1: 关闭 LED
uint8_t button1_flag;
uint8_t button2_flag;

#define DEBOUNCE_TIME 200 // 按键防抖延迟，单位为毫秒

// 简单的延时函数
void delay(uint32_t s) {
    while(s--) {
        __NOP(); // 空操作来消耗时间
    }
}

// 主程序
int main(void) {
    GPIO_Ini_1(); // 初始化 GPIO

    while (1) {
        // 按钮1处理（PC12）
         button1_flag =! button1_active;
         button2_flag =! button2_active;
        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) && (button1_active == 0)) { 
            delay(DEBOUNCE_TIME); // 防抖延迟
            if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) {
                current_mode ^= 1; // 切换 LED 模式
                button1_active = 1; // 设置按钮1为已按下状态
            }
        } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) == 0) {
            button1_active = 0; // 重置按钮1状态
        }

        // 按钮2处理（PC13）
        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) && (button2_active == 0)) { 
            delay(DEBOUNCE_TIME); // 防抖延迟
            if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) {
                // 根据当前模式处理 LED 状态
                if (current_mode == 0) { // 打开 LED
                    if (led_index < 3) {
                        switch (led_index) {
                            case 0:
                                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); // 点亮 LED1
                                break;
                            case 1:
                                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); // 点亮 LED2
                                break;
                            case 2:
                                SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET); // 点亮 LED3
                                break;
                        }
                        led_index++; // 更新 LED 状态
                    }
                } else { // 关闭 LED
                    if (led_index > 0) {
                        switch (led_index) {
                            case 1:
                               SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR0); // 熄灭 LED1
                                break;
                            case 2:
                                SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR7); // 熄灭 LED2
                                break;
                            case 3:
                               SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR14); // 熄灭 LED3
                                break;
                        }
                        led_index--; // 更新 LED 状态
                    }
                }

                button2_active = 1; // 更新按钮2状态
            }
        } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0) {
            button2_active = 0; // 重置按钮2状态
        }

        delay(20000); // 稍微延时，防止过快的循环
    }
}