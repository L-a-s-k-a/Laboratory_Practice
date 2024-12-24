#include "it_handlers.h"

void SysTick_Handler(void) {
    // SysTick中断处理
    tick_count++; // 增加SysTick计数

    // 处理按钮按下的时间
    if (button_pressed) {
        button_press_time++; // 增加按下时间计数
    }

    // 根据当前模式控制LED的切换
    if (mode == 2 || mode == 3) {
        if (tick_count >= (mode == 2 ? 2500 : 3333)) { // 根据模式设置切换时间
            tick_count = 0; // 重置计数
            led_index++; // 增加LED索引
            if (mode == 2) {
                if (led_index > 5) {
                    led_index = 0; // 重置索引
                }
            } else if (mode == 3) {
                if (led_index > 2) {
                    led_index = 0; // 重置索引
                }
            }
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    if (READ_BIT(EXTI->PR, EXTI_PR_PR13)) { // 检查是否为按钮引脚的中断
        if (button_pressed == 0) { // 按钮按下
            button_pressed = 1; // 更新按钮状态为已按下
            button_press_time = 0; // 重置按下时间
        } else { // 按钮释放
            if (button_press_time < 2000) { // 短按（小于2秒）
                mode++; // 改变模式
                if (mode > 3) {
                    mode = 1; // 循环到模式1
                }
            } else if (button_press_time >= 2000) { // 长按（大于等于2秒）
                frequency_mode++; // 增加频率模式
                if (frequency_mode > 2) {
                    frequency_mode = 0; // 循环到频率模式0
                }
                // 不改变模式
            }
            button_pressed = 0; // 重置按钮状态
        }

        EXTI->PR |= EXTI_PR_PR13; // 清除中断标志
    }
}

// extern uint8_t BtnCount1; // 外部变量
// extern uint16_t GlobalTickCount;

// uint16_t ExternInterruptTickCount;
// uint8_t front1;

// void SysTick_Handler(void)
// {
//     ExternInterruptTickCount++;
//     GlobalTickCount++;
// }

// void EXTI15_10_IRQHandler(void)
// {
//     SET_BIT(EXTI->PR, EXTI_PR_PR13); // 清除中断标志
//     if (ExternInterruptTickCount >= DELAY_BUTTON_FILTER)
//     {
//         front1 = !front1;
//         if (front1 == 1)
//             dTime1 = GlobalTickCount;
//         else
//         {
//             if (GlobalTickCount - (dTime1 - 1) >= 2001)
//                 BtnCount1 += 2; // 长按
//             else
//                 BtnCount1 = 1; // 短按
//         }
//         ExternInterruptTickCount = 0; // 重置计时器
//     }
// }