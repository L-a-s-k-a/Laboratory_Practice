#include "init.h"
#include "it_handlers.h"

const uint32_t mode1_frequencies[] = {1666, 454, 333}; // 0.6Hz, 2.2Hz, 3Hz (单位：毫秒)
const uint32_t mode2_frequencies[] = {2000, 833, 500}; // 0.5Hz, 1.2Hz, 2Hz (单位：毫秒)

volatile uint8_t current_mode = 1;      // 当前模式：1 或 2
volatile uint8_t current_led = 0;      // 当前设置频率的LED编号
volatile uint8_t current_frequency = 0; // 当前频率索引
volatile uint32_t systick_counter = 0; // SysTick计数器

uint8_t flagbutton1, flagbutton2, flagbutton3;

int main(void) {
    RCC_Ini();
    GPIO_Ini();
    EXTI_ITR_Ini();
    SysTick_Init();

    while (1) {
        // 读取按钮状态
        flagbutton1 = READ_BIT(GPIOC->IDR, GPIO_IDR_ID8) != 0;
        flagbutton2 = READ_BIT(GPIOC->IDR, GPIO_IDR_ID9) != 0;
        flagbutton3 = READ_BIT(GPIOC->IDR, GPIO_IDR_ID10) != 0;

        // 处理按钮1（模式切换）
        if (flagbutton1) {
            current_mode = (current_mode % 2) + 1; // 切换模式
        }

        // 处理按钮2（频率调整）
        if (flagbutton2) {
            current_frequency = (current_frequency + 1) % 3; // 切换频率
        }

        // 处理按钮3（LED编号切换，用于模式2）
        if (flagbutton3 && current_mode == 2) {
            current_led = (current_led + 1) % 6; // 切换LED编号
        }

        if (current_mode == 1) { // 模式 1：两灯一组按顺序点亮并熄灭
            static uint32_t last_toggle = 0;
            static uint8_t stage = 0;

            if (systick_counter - last_toggle >= mode1_frequencies[current_frequency]) {
                last_toggle = systick_counter;

                if (stage < 3) {
                    update_led_pair(stage);
                    stage++;
                } else {
                    reverse_led_pair(stage - 1);
                    stage--;
                }
            }
        } else if (current_mode == 2) { // 模式 2：单灯闪烁并调整频率
            static uint32_t last_blink[6] = {0};
            for (int i = 0; i < 6; i++) {
                if (systick_counter - last_blink[i] >= mode2_frequencies[current_frequency]) {
                    last_blink[i] = systick_counter;
                    toggle_led(i);
                }
            }
        }
    }
}
