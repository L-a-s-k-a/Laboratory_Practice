#include "init.h"

#define DEBOUNCE_DELAY 20000 // 防抖延迟
#define LIGHT_DELAY 500000   // 每个LED点亮后的延迟时间

void delay(uint32_t s) {
    while (s--) {
        __NOP(); // 空循环
    }
}

uint8_t flag1 = 0;
uint8_t flag2 = 0;
uint8_t flag3 = 0;

int main(void) {
    GPIO_Ini(); // 初始化 GPIO
    uint8_t leds[] = {BOARD_LED1_PIN, BOARD_LED2_PIN, BOARD_LED3_PIN, CN8_LED4_PIN, CN8_LED5_PIN, CN8_LED6_PIN};
    uint8_t led_state = 0;   // 当前点亮的 LED
    uint8_t swap = 0;        // 功能交换标志
    uint8_t button1_function = 1; // 按钮1功能（点亮）
    uint8_t button2_function = 0; // 按钮2功能（熄灭）

    while (1) {
        flag1 = (READ_BIT(GPIOC_IDR, (1 << BUTTON1_PIN)) != 0);
        flag2 = (READ_BIT(GPIOC_IDR, (1 << BUTTON2_PIN)) != 0);
        flag3 = (READ_BIT(GPIOC_IDR, (1 << BUTTON3_PIN)) != 0);

        // 检测按钮1
        if (flag1) {
            delay(DEBOUNCE_DELAY);
            if (flag1) {
                if (button1_function) { // 点亮模式
                    if (led_state < 6) {
                        SET_BIT(GPIOB_BSRR, (1 << leds[led_state])); // 点亮 LED
                        delay(LIGHT_DELAY); // 添加延迟以体现逐个点亮
                        led_state++;
                    }
                } else { // 熄灭模式
                    if (led_state > 0) {
                        led_state--;
                        delay(LIGHT_DELAY); // 添加延迟以体现逐个熄灭
                        SET_BIT(GPIOB_BSRR, (1 << (leds[led_state] + 16))); // 熄灭 LED
                    }
                }
            }
        }

        // 检测按钮2
        if (flag2) {
            delay(DEBOUNCE_DELAY);
            if (flag2) {
                if (button2_function) { // 点亮模式
                    if (led_state < 6) {
                        SET_BIT(GPIOB_BSRR, (1 << leds[led_state])); // 点亮 LED
                        delay(LIGHT_DELAY); // 添加延迟以体现逐个点亮
                        led_state++;
                    }
                } else { // 熄灭模式
                    if (led_state > 0) {
                        led_state--;
                        delay(LIGHT_DELAY); // 添加延迟以体现逐个熄灭
                        SET_BIT(GPIOB_BSRR, (1 << (leds[led_state] + 16))); // 熄灭 LED
                    }
                }
            }
        }

        // 检测按钮3
        if (flag3) {
            delay(DEBOUNCE_DELAY);
            if (flag3) {
                // 交换功能
                button1_function ^= 1;
                button2_function ^= 1;
            }
        }
    }
}
