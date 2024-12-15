#include "init.h"

int main(void) {
    // 初始化GPIO
    GPIO_Ini_1();
    GPIO_Ini_2();
    
    // 按钮状态变量
    uint32_t button1_status = 0;
    uint32_t button2_status = 0;
    uint32_t button3_status = 0;

    // 主循环
    while (1) {
        // 读取按钮状态（按下时返回非零值）
        button1_status = (GPIOC_IDR & GPIOC_IDR_PIN13) ? 1 : 0;
        button2_status = (GPIOC_IDR & GPIOC_IDR_PIN12) ? 1 : 0;

        if (button1_status) {
            // 设置端口为输入模式，点亮红色LED
            LED_On(GPIOB_BSRR_PIN0_SET);
        } else {
            LED_Off(GPIOB_BSRR_PIN0_RESET);
        }

        if (button2_status) {
            // 设置端口为输出模式，点亮蓝色LED
            LED_On(GPIOB_BSRR_PIN7_SET);
        } else {
            LED_Off(GPIOB_BSRR_PIN7_RESET);
        }

        // 检查按钮3的状态，控制黄色和白色LED
        button3_status = (GPIOC_IDR & GPIOC_IDR_PIN13) ? 1 : 0;
        if (button3_status) {
            if (button1_status) {
                // 端口为输入模式，点亮黄色LED
                LED_On(GPIOB_BSRR_PIN14_SET);
            } else {
                // 端口为输出模式，点亮白色LED
                LED_On(GPIOB_BSRR_PIN8_SET);
            }
        } else {
            LED_Off(GPIOB_BSRR_PIN14_RESET);
            LED_Off(GPIOB_BSRR_PIN8_RESET);
        }

        // 延迟
        for (volatile int i = 0; i < 1000000; i++);
    }
}
