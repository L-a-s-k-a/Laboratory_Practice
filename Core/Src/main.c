#include <stdint.h> 
#include "init.h" 
#include "init.c"

int main(void)
{ 
   GPIO_Ini_1();

    uint8_t led_state = 0; // LED 状态变量，0: LED1, 1: LED2, 2: LED3,3: LED4
    uint8_t button_pressed = 0; // 按钮1状态变量，0: 未按下, 1: 按下
    uint8_t button2_pressed = 0; // 按钮2状态变量
    uint8_t mode = 0; // 按钮1的工作模式，0: 点亮LED，1: 熄灭LED

    while (1) {
        // 检查按钮状态
        if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0) // 按钮按下时，PC13 为 0
        { 
            if (!button_pressed)  // 仅在状态变化时执行
            { 
                // 根据当前状态点亮下一个 LED
                if (led_state == 0) {
                    SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); // 点亮 LED1
                    led_state = 1; // 更新状态
                } else if (led_state == 1) {
                    SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); // 点亮 LED2
                    led_state = 2; // 更新状态
                } else if (led_state == 2) {
                    SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET); // 点亮 LED3
                    led_state = 3; // 更新状态，表示所有LED已点亮
                } else if (led_state == 3) {
                    // 熄灭所有 LED 并重置状态
                    SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET);
                    led_state = 0; // 重置状态，准备重新开始
                }

                // 更新按钮状态为已按下
                button_pressed = 1;
            }
        } else {
            // 如果按钮未按下，重置按钮状态
            button_pressed = 0;
        }
        delay(20000);
    
    }
    
}

// int main(void) {
//     GPIO_Ini_1();

//     uint8_t led_state = 0; // LED 状态变量，0: LED1, 1: LED2, 2: LED3, 3: LED4
//     uint8_t button1_pressed = 0; // 按钮1状态变量
//     uint8_t button2_pressed = 0; // 按钮2状态变量
//     uint8_t mode = 0; // 按钮1的工作模式，0: 点亮LED，1: 熄灭LED

//     while (1) {
//         // 检查按钮1状态（假设连接到PC13）
//         if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) { // 按钮1按下时，PC13 为 0
//             if (!button1_pressed) { // 仅在状态变化时执行
//                 delay(DEBOUNCE_DELAY); // 防抖延迟
//                 if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) {
//                     // 切换模式
//                     mode ^= 1; // 切换模式
//                     button1_pressed = 1; // 更新按钮状态为已按下
//                 }
//             }
//         } else {
//             button1_pressed = 0; // 重置按钮状态
//         }

//         // 检查按钮2状态（假设连接到PC12）
//         if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) == 0) { // 按钮2按下时，PC12 为 0
//             if (!button2_pressed) { // 仅在状态变化时执行
//                 delay(DEBOUNCE_DELAY); // 防抖延迟
//                 if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) == 0) {
//                     // 根据当前模式点亮或熄灭LED8
//                     if (mode == 0) {
//                         SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN8_SET); // 点亮 LED4
//                     } else {
//                         SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN8_RESET); // 熄灭 LED4
//                     }
//                     button2_pressed = 1; // 更新按钮状态为已按下
//                 }
//             }
//         } else {
//             button2_pressed = 0; // 重置按钮状态
//         }

//         // 检查LED状态并依次点亮LED
//         if (led_state < 4) {
//             if (led_state == 0) {
//                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); // 点亮 LED1
//             } else if (led_state == 1) {
//                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); // 点亮 LED2
//             } else if (led_state == 2) {
//                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET); // 点亮 LED3
//             } else if (led_state == 3) {
//                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN8_SET); // 点亮 LED4
//             }
//             led_state++; // 更新状态
//         } else {
//             // 熄灭所有 LED 并重置状态
//             SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | GPIOB_BSRR_PIN8_RESET);
//             led_state = 0; // 重置状态，准备重新开始
//         }

//         delay(20000); // 延时，防止快速循环
//     }
// }

// int main(void) {
//     GPIO_Ini_1();

//     uint8_t led_state = 0; // LED 状态变量，0: LED1, 1: LED2, 2: LED3
//     uint8_t button1_pressed = 0; // 按钮1状态变量
//     uint8_t button2_pressed = 0; // 按钮2状态变量
//     uint8_t mode = 0; // 按钮1的工作模式，0: 打开LED，1: 关闭LED

//     while (1) {
//         // 检查按钮1状态（假设连接到PC12）
//         if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) { // 按钮1按下时，PC12 为 0
//             if (!button1_pressed) { // 仅在状态变化时执行
//                 delay(DEBOUNCE_DELAY); // 防抖延迟
//                 if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) == 0) {
//                     // 切换模式
//                     mode ^= 1; // 切换模式
//                     button1_pressed = 1; // 更新按钮状态为已按下
//                 }
//             }
//         } else {
//             button1_pressed = 0; // 重置按钮状态
//         }

//         // 检查按钮2状态（假设连接到PC13）
//         if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0) { // 按钮2按下时，PC13 为 0
//             if (!button2_pressed) { // 仅在状态变化时执行
//                 delay(DEBOUNCE_DELAY); // 防抖延迟
//                 if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0) {
//                     // 切换LED的状态
//                     if (mode == 0) {
//                         // 打开LED
//                         if (led_state < 3) {
//                             if (led_state == 0) {
//                                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); // 点亮 LED1
//                             } else if (led_state == 1) {
//                                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); // 点亮 LED2
//                             } else if (led_state == 2) {
//                                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET); // 点亮 LED3
//                             }
//                             led_state++; // 更新状态
//                         }
//                     } else {
//                         // 关闭LED
//                         if (led_state > 0) {
//                             if (led_state == 1) {
//                                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET); // 熄灭 LED1
//                             } else if (led_state == 2) {
//                                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET); // 熄灭 LED2
//                             } else if (led_state == 3) {
//                                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_RESET); // 熄灭 LED3
//                             }
//                             led_state--; // 更新状态
//                         }
//                     }
//                     button2_pressed = 1; // 更新按钮状态为已按下
//                 }
//             }
//         } else {
//             button2_pressed = 0; // 重置按钮状态
//         }

//         // 检查是否所有LED都打开或关闭
//         if (led_state == 3 && mode == 0) {
//             // 所有LED都点亮，停止操作
//             while (1) { /* 停止操作 */ }
//         } else if (led_state == 0 && mode == 1) {
//             // 所有LED都熄灭，停止操作
//             while (1) { /* 停止操作 */ }
//         }

//         delay(20000); // 延时，防止快速循环
//     }
// }


//实验2
