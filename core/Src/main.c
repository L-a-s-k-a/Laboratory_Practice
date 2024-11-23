#include "init.h"

void delay(volatile uint32_t s) {
    while (s--);
}

// lp1
uint8_t current_led = 0;        // 当前点亮的 LED 起始编号（0=LED1，1=LED2，2=LED3）
uint8_t led_count = 1;          // 每次点亮的 LED 数量（1-3）
uint8_t button1_last_state = 1; // 按钮1上次状态
uint8_t button2_last_state = 1; // 按钮2上次状态

// 更新 LED 状态函数
void update_led_state(void) {
    // 关闭所有 LED
    SET_BIT_SELF(GPIOB_BSRR, LED1_OFF | LED2_OFF | LED3_OFF);

    // 根据 led_count 和 current_led 点亮 LED
    for (uint8_t i = 0; i < led_count; i++) {
        uint8_t led_to_light = (current_led + i) % 3; // 循环点亮 LED
        if (led_to_light == 0) SET_BIT_SELF(GPIOB_BSRR, LED1_ON);
        if (led_to_light == 1) SET_BIT(GPIOB->BSRR,GPIO_BSRR_BS14);
        if (led_to_light == 2) SET_BIT(GPIOB->BSRR,GPIO_BSRR_BS0);
    }
}

int main(void) {
    // 初始化 GPIO
    GPIO_Ini_Self_Def();
    GPIO_Ini_CMSIS();

    while (1) {
        // 读取按钮状态
        uint8_t button1_current_state = !(READ_BIT_SELF(GPIOC_IDR, GPIO_PIN_13) != 0);
        uint8_t button2_current_state = !(READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0);

        // 按钮1按下：切换当前点亮的 LED
        if (button1_last_state == 1 && button1_current_state == 0) {
            current_led = (current_led + led_count) % 3; // 循环切换 LED
            update_led_state();                         // 更新 LED 状态
        }
        button1_last_state = button1_current_state;

        // 按钮2按下：调整每次点亮的 LED 数量
        if (button2_last_state == 1 && button2_current_state == 0) {
            led_count = (led_count % 3) + 1; // 循环设置 LED 数量（1-3）
            update_led_state();             // 更新 LED 状态
        }
        button2_last_state = button2_current_state;

        // 简单防抖延迟
        delay(100000);
    }
}





// int main(void)
// {
//     GPIO_Ini_Self_Def();
//     GPIO_Ini_CMSIS();

//     while(1)
//     {
       
//         if(READ_BIT_SELF(GPIOC_IDR,GPIO_PIN_13) !=0) /*使用gpioc中idr寄存器读取pc13的电平即按钮 !=不等于 */
//         {
//             SET_BIT_SELF(GPIOB_BSRR,GPIO_PIN_7_SET); /*使用gpiob中bsrr寄存器点亮pb7蓝色led */
//         }
//         else
//         {
//             SET_BIT_SELF(GPIOB_BSRR,GPIO_PIN_7_RESET);
//         }


//         if(READ_BIT(GPIOC->IDR,GPIO_IDR_ID13)!=0)  /*使用gpioc中idr寄存器读取pc13的电平即按钮（使用cmsis点亮） */
//         {
//             SET_BIT(GPIOB->BSRR,GPIO_BSRR_BS14); /*使用gpiob中bsrr寄存器点亮pb14红色led */
//             SET_BIT(GPIOB->BSRR,GPIO_BSRR_BS0); /*使用gpiob中bsrr寄存器点亮pb0绿色led */
//         }
//         else
//         {
//             SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR14);
//             SET_BIT(GPIOB->BSRR,GPIO_BSRR_BR0);
//         }
//     }
// }