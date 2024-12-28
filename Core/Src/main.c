#include "init.h"

void delay(volatile uint32_t s) {
    while (s--);
}

uint8_t led_count = 1;         // 按钮2操作时点亮的LED数量
uint8_t current_led = 0;       // 当前点亮的LED索引
uint32_t led_states[] = {LED1_ON, LED2_ON, LED3_ON};

uint8_t toggle_state = 0;      // 记录LED开关状态，用于数量为3的模式
uint8_t button1_flag;
uint8_t button2_flag;
int main() {
    GPIO_Ini_Self_Def();
    GPIO_Ini_CMSIS();

    uint8_t button1_pressed = 0;
    uint8_t button2_pressed = 0;
    

    while (1) {
        // 按钮1逻辑
        button1_flag=!button1_pressed;
        button2_flag=!button2_pressed;
        if (!(READ_BIT_SELF(GPIOC_IDR, GPIO_PIN_13) != 0)) {
            if (!button1_pressed) {
                button1_pressed = 1;

                if (led_count == 3) {
                    // 如果LED数量为3，按下按钮切换开关状态
                    toggle_state = !toggle_state;
                    update_leds(current_led, led_count);
                } else {
                    // 否则，切换到下一个LED
                    current_led = (current_led + 1) % 3;  // 切换到下一个LED
                    update_leds(current_led, led_count);
                }
            }
        } else {
            button1_pressed = 0;
        }

        // 按钮2逻辑
        if (!(READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0)) {
            if (!button2_pressed) {
                button2_pressed = 1;
                led_count = (led_count % 3) + 1;  // 切换LED数量 (1->2->3->1)
                update_leds(current_led, led_count);
            }
        } else {
            button2_pressed = 0;
        }
        
        delay(1000);  // 去抖
    }
}

// 更新LED状态
void update_leds(uint8_t led_index, uint8_t count) {
    // 关闭所有LED
    GPIOB->BSRR = (LED1_ON | LED2_ON | LED3_ON) << 16;

    if (count == 3) {
        // 如果LED数量为3，根据 toggle_state 控制所有灯
        if (toggle_state) {
            GPIOB->BSRR = LED1_ON | LED2_ON | LED3_ON;  // 打开所有灯
        }
    } else {
        // 点亮指定数量的LED
        for (uint8_t i = 0; i < count; i++) {
            uint8_t index = (led_index + i) % 3;  // 循环点亮
            GPIOB->BSRR = led_states[index];
        }
    }
}