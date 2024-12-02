#include <stdint.h> 
#include "init.h" 
// 延时函数，简单的循环延时（可根据需要调整）
void delay(uint32_t count) {
    while (count--) {
        __asm("NOP");  // 空操作，等待
    }
}

int main(void)
{
    GPIO_Ini_1();  // 初始化GPIO

    uint8_t led_state = 0;       // LED 状态变量，0: LED1, 1: LED2, 2: LED3, 3: 所有LED熄灭
    uint8_t button1_pressed = 0; // 按钮1状态变量
    uint8_t button2_pressed = 0; // 按钮2状态变量
    uint8_t mode = 0; // 按钮1的工作模式，0: 点亮LED，1: 熄灭LED

    while (1) {
        // 检查按钮1状态（假设连接到PC13）
        if ((GPIOC_IDR & GPIOC_IDR_PIN13) == 0) { // 按钮1按下时，PC13 为 0
            if (!button1_pressed) { // 仅在状态变化时执行
                delay(20000); // 防抖动延时
                if (led_state == 0) {
                    GPIOB_BSRR = GPIOB_BSRR_PIN0_SET; // 点亮 LED1
                    led_state = 1;
                } else if (led_state == 1) {
                    GPIOB_BSRR = GPIOB_BSRR_PIN7_SET; // 点亮 LED2
                    led_state = 2;
                } else if (led_state == 2) {
                    GPIOB_BSRR = GPIOB_BSRR_PIN14_SET; // 点亮 LED3
                    led_state = 3;
                } else if (led_state == 3) {
                    // 熄灭所有 LED 并重置状态
                    GPIOB_BSRR = (GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET);
                    led_state = 0;
                }
                button1_pressed = 1; // 按钮1已按下
            }
        } else {
            button1_pressed = 0; // 重置按钮1状态
        }

        // 检查按钮2状态（假设连接到PC12）
        if ((GPIOC_IDR & GPIOC_IDR_PIN12) == 0) { // 按钮2按下时，PC12 为 0
            if (!button2_pressed) { // 仅在状态变化时执行
                delay(20000); // 防抖动延时
                // 按钮2功能切换代码（可以在这里实现按钮2的功能）
                button2_pressed = 1; // 按钮2已按下
            }
        } else {
            button2_pressed = 0; // 重置按钮2状态
        }
    }
}
