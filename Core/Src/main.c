#include <stdint.h> 
#include "init.h" 
#include "init.c" 

// uint8_t led_state = 0; // LED 状态变量
//     uint8_t button1_pressed = 0; // 按钮1状态变量
//     uint8_t button2_pressed = 0; // 按钮2状态变量
//     uint8_t mode = 0; // 按钮2的工作模式，0: 开启LED，1: 关闭LED
//     uint8_t button1_flag;
//     uint8_t button2_flag;

// //实验1
// #define DEBOUNCE_DELAY 20 // 防抖延迟时间，单位为毫秒

// void delay(uint32_t s){
//     while(s--);
// }

// int main(void) {
    
//     GPIO_Ini_1();

    

//     while (1) {
//         button1_flag = button1_pressed;
//         button2_flag = button2_pressed;
//         // 检查按钮1状态（PC12）
//         if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) && !button1_pressed) { // 按钮1按下时，PC12 为 0
//             delay(DEBOUNCE_DELAY); // 防抖延迟
//             if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) {
//                 mode ^= 1; // 切换模式
//                 button1_pressed = 1; // 更新按钮状态为已按下
//             }
//         } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) {
//             button1_pressed = 0; // 重置按钮状态
//         }

//         // 检查按钮2状态（PC13）
//         if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) && !button2_pressed) { // 按钮2按下时，PC13 为 0
//             delay(DEBOUNCE_DELAY); // 防抖延迟
//             if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) {
//                 // 根据模式切换LED状态
//                 if (mode == 0) { // 打开模式
//                     if (led_state < 3) {
//                         if (led_state == 0) {
//                             SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); // 点亮 LED1
//                         } else if (led_state == 1) {
//                             SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); // 点亮 LED2
//                         } else if (led_state == 2) {
//                             SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET); // 点亮 LED3
//                         }
//                         led_state++; // 更新状态
//                     }
//                 } else { // 关闭模式
//                     if (led_state > 0) {
//                         if (led_state == 1) {
//                             SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR0);
//                             //SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET); // 熄灭 LED1
//                         } else if (led_state == 2) {
//                             SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR7);
//                             //SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET); // 熄灭 LED2
//                         } else if (led_state == 3) {
//                             SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR14);
//                             //SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_RESET); // 熄灭 LED3
//                         }
//                         led_state--; // 更新状态
//                     }
//                 }

//                 button2_pressed = 1; // 更新按钮状态为已按下
//             }
//         } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0) {
//             button2_pressed = 0; // 重置按钮状态
//         }

//         delay(20000); // 延时，防止快速循环
//     }
// }




uint8_t led_state = 0; // LED 状态变量
uint8_t button1_pressed = 0; // 按钮1状态变量
uint8_t button2_pressed = 0; // 按钮2状态变量
uint8_t mode = 0; // 按钮2的工作模式，0: 开启LED，1: 关闭LED

uint8_t button1_flag;
uint8_t button2_flag;

#define DEBOUNCE_DELAY 200 // 防抖延迟时间，单位为毫秒

void delay(uint32_t s){
    while(s--);
}

int main(void) {
    GPIO_Ini_1(); // 初始化GPIO端口

    while (1) {
        // 检查按钮1状态（PC12）

         button1_flag =! button1_pressed;
         button2_flag =! button2_pressed;
        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) && !button1_pressed) { // 按钮1按下时，PC12 为 0
            delay(DEBOUNCE_DELAY); // 防抖延迟
            if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) {
                mode ^= 1; // 切换模式
                button1_pressed = 1; // 更新按钮状态为已按下
            }
        } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) == 0) {
            button1_pressed = 0; // 重置按钮状态
        }

        // 检查按钮2状态（PC13）
        if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) && !button2_pressed) { // 按钮2按下时，PC13 为 0
            delay(DEBOUNCE_DELAY); // 防抖延迟
            if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) == 0) {
                // 根据模式切换LED状态
                if (mode == 0) { // 打开模式
                    if (led_state < 3) {
                        if (led_state == 0) {
                            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); // 点亮 LED1
                        } else if (led_state == 1) {
                            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); // 点亮 LED2
                        } else if (led_state == 2) {
                            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET); // 点亮 LED3
                        }
                        led_state++; // 更新状态
                    }
                } else { // 关闭模式
                    if (led_state > 0) {
                        if (led_state == 1) {
                            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0); // 熄灭 LED1
                        } else if (led_state == 2) {
                            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); // 熄灭 LED2
                        } else if (led_state == 3) {
                            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14); // 熄灭 LED3
                        }
                        led_state--; // 更新状态
                    }
                }

                button2_pressed = 1; // 更新按钮状态为已按下
            }
        } else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0) {
            button2_pressed = 0; // 重置按钮状态
        }

        delay(20000); // 延时，防止快速循环
    }
}
