#include "init.h"

int main(void) {
    uint8_t led_state = 0;        // LED 状态
    uint8_t button1_press_count = 0; // 按钮1按下次数
    uint8_t button1_state = 0;    // 按钮1当前状态
    uint8_t button2_state = 0;    // 按钮2当前状态
    uint8_t port_mode = 0;        // 按钮1的 LED 独立控制模式

    GPIO_Init();

    while (1) {
        // 检测按钮1（PC8）
        if ((GPIOC_IDR & GPIOC_IDR_PIN8) == 0 && button1_state == 0) {
            delay(20000); // 防抖延迟
            if ((GPIOC_IDR & GPIOC_IDR_PIN8) == 0) {
                button1_press_count++;
                button1_state = 1;

                // 每第 5 次按下切换模式
                if (button1_press_count % 5 == 0) {
                    port_mode ^= 1; // 切换模式
                }

                // 根据模式控制 LED1 独立开关
                if (port_mode == 1) {
                    GPIOC_BSRR ^= GPIOC_BSRR_LED1_ON | GPIOC_BSRR_LED1_OFF; // 切换 LED1 状态
                }
            }
        } else if ((GPIOC_IDR & GPIOC_IDR_PIN8) != 0) {
            button1_state = 0; // 按钮释放
        }

        // 检测按钮2（PC9）
        if ((GPIOC_IDR & GPIOC_IDR_PIN9) == 0 && button2_state == 0) {
            delay(20000); // 防抖延迟
            if ((GPIOC_IDR & GPIOC_IDR_PIN9) == 0) {
                button2_state = 1;

                // 顺序控制 LED3 和 LED4
                led_state = (led_state + 1) % 4; // 循环状态
                GPIOC_BSRR |= GPIOC_BSRR_LED1_OFF | GPIOC_BSRR_LED2_OFF |
                              GPIOC_BSRR_LED3_OFF | GPIOC_BSRR_LED4_OFF; // 熄灭所有灯

                switch (led_state) {
                    case 0:
                        GPIOC_BSRR |= GPIOC_BSRR_LED3_ON;
                        break;
                    case 1:
                        GPIOC_BSRR |= GPIOC_BSRR_LED4_ON;
                        break;
                    case 2:
                        GPIOC_BSRR |= GPIOC_BSRR_LED1_ON;
                        break;
                    case 3:
                        GPIOC_BSRR |= GPIOC_BSRR_LED2_ON;
                        break;
                }
            }
        } else if ((GPIOC_IDR & GPIOC_IDR_PIN9) != 0) {
            button2_state = 0; // 按钮释放
        }

        delay(200000); // 循环延迟
    }
}
