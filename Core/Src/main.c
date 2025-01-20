#include <stdint.h> 
#include "init.h" 
#include "init.c" 
#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"
#include "it_handlers.h"

volatile uint8_t mode = 1; // 模式标记，初始为模式1
volatile uint8_t led_index = 0; // 当前LED索引
volatile uint32_t button_press_time = 0; // 按钮按下时间
volatile uint8_t button_pressed = 0; // 按钮状态标记
volatile uint8_t frequency_mode = 0; // 频率模式标记
volatile uint32_t tick_count = 0; // SysTick计数

int main(void) {
    GPIO_Ini(); // 初始化GPIO
    EXTI_Ini(); // 初始化外部中断
    SysTick_Ini(); // 初始化SysTick

    // 初始状态：所有LED熄灭
    GPIOB->BSRR |= (GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | 
                    GPIOB_BSRR_PIN8_RESET | GPIOB_BSRR_PIN9_RESET | GPIOB_BSRR_PIN10_RESET);

    while (1) {
        // 根据模式控制LED状态
        if (mode == 1) {
            // 模式1：所有LED点亮
            GPIOB->BSRR |= (GPIOB_BSRR_PIN0_SET | GPIOB_BSRR_PIN7_SET | GPIOB_BSRR_PIN14_SET | 
                            GPIOB_BSRR_PIN8_SET | GPIOB_BSRR_PIN9_SET | GPIOB_BSRR_PIN10_SET);
        } else if (mode == 2) {
            // 模式2：逐个亮起
            GPIOB->BSRR |= (GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | 
                            GPIOB_BSRR_PIN8_RESET | GPIOB_BSRR_PIN9_RESET | GPIOB_BSRR_PIN10_RESET);
            switch (led_index) {
                case 0: GPIOB->BSRR |= GPIOB_BSRR_PIN0_SET; break;
                case 1: GPIOB->BSRR |= GPIOB_BSRR_PIN7_SET; break;
                case 2: GPIOB->BSRR |= GPIOB_BSRR_PIN14_SET; break;
                case 3: GPIOB->BSRR |= GPIOB_BSRR_PIN8_SET; break;
                case 4: GPIOB->BSRR |= GPIOB_BSRR_PIN9_SET; break;
                case 5: GPIOB->BSRR |= GPIOB_BSRR_PIN10_SET; break;
            }
        } else if (mode == 3) {
            // 模式3：成对亮起
            GPIOB->BSRR |= (GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | 
                            GPIOB_BSRR_PIN8_RESET | GPIOB_BSRR_PIN9_RESET | GPIOB_BSRR_PIN10_RESET);
            switch (led_index) {
                case 0: GPIOB->BSRR |= (GPIOB_BSRR_PIN0_SET | GPIOB_BSRR_PIN8_SET); break; // 点亮LED1和LED4
                case 1: GPIOB->BSRR |= (GPIOB_BSRR_PIN7_SET | GPIOB_BSRR_PIN9_SET); break; // 点亮LED2和LED5
                case 2: GPIOB->BSRR |= (GPIOB_BSRR_PIN14_SET | GPIOB_BSRR_PIN10_SET); break; // 点亮LED3和LED6
            }
        }
    }
}


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




// #include "../Inc/init.h"

// uint8_t mode = 0; // 0: 全亮, 1: 逐个亮, 2: 成对亮
// uint8_t flag1ON, flag2ON, flag3ON, flag4ON, flag5ON, flag6ON;
// uint8_t BtnCount1 = 0; // 初始化按钮计数
// uint16_t GlobalTickCount = 0; // 初始化全局计数
// uint16_t dtime;
// int freqMode1[3] = {2500, 526, 400}; // 频率 (0.4Hz, 1.9Hz, 2.6Hz)
// int freqMode2[3] = {3333, 625, 434}; // 频率 (0.3Hz, 1.6Hz, 2.3Hz)
// int currentFreqIndex = 0;

// int main(void)
// {
//     ITR_init();     // 初始化中断
//     RCC_Init();     // 初始化时钟
//     GPIO_Ini();     // 初始化GPIO
//     SysTick_Init(); // 初始化系统定时器

//     while (1)
//     {
//         // 更新按钮长按计数
//         if (BtnCount1 > 0)
//         {
//             // 短按切换模式
//             if (BtnCount1 == 1)
//             {
//                 mode = (mode + 1) % 3; // 切换模式
//             }
//             // 长按改变频率
//             else if (BtnCount1 >= 2)
//             {
//                 if (mode == 1)
//                 {
//                     currentFreqIndex = (currentFreqIndex + 1) % 3; // 循环频率
//                 }
//                 else if (mode == 2)
//                 {
//                     currentFreqIndex = (currentFreqIndex + 1) % 3; // 循环频率
//                 }
//             }
//             BtnCount1 = 0; // 重置按钮计数
//         }

//         // 计算时间增量
//         dtime = GlobalTickCount;

//         // LED控制逻辑
//         uint16_t freq1 = (mode == 1) ? freqMode1[currentFreqIndex] : freqMode2[currentFreqIndex];

//         switch (mode)
//         {
//         case 0: // 全部亮起
//             flag1ON = flag2ON = flag3ON = flag4ON = flag5ON = flag6ON = 1;
//             break;

//         case 1: // 逐个亮起
//             flag1ON = (dtime / freq1) % 2; // 每隔一定时间切换状态
//             flag2ON = (dtime / freq1 + 1) % 2;
//             flag3ON = (dtime / freq1 + 2) % 2;
//             flag4ON = (dtime / freq1 + 3) % 2;
//             flag5ON = (dtime / freq1 + 4) % 2;
//             flag6ON = (dtime / freq1 + 5) % 2;
//             break;

//         case 2: // 成对亮起
//             flag1ON = (dtime / freq1) % 2;
//             flag4ON = (dtime / freq1) % 2;
//             flag2ON = (dtime / freq1 + 1) % 2;
//             flag5ON = (dtime / freq1 + 1) % 2;
//             flag3ON = (dtime / freq1 + 2) % 2;
//             flag6ON = (dtime / freq1 + 2) % 2;
//             break;
//         }

//         // 控制LED输出
//         if (flag1ON)
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0); // PB0
//         else
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0); // PB0

//         if (flag2ON)
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // PB7
//         else
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); // PB7

//         if (flag3ON)
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14); // PB14
//         else
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14); // PB14

//         if (flag4ON)
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8); // PB8
//         else
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8); // PB8

//         if (flag5ON)
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS9); // PB9
//         else
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR9); // PB9

//         if (flag6ON)
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS10); // PB10
//         else
//             SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10); // PB10
//     }
// }
