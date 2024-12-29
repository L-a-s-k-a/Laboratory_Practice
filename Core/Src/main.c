#include <stdint.h> 
#include <init.h> 

// // // 函数声明
// // void Init_GPIO(void);           // 初始化GPIO
// // void ProcessButtons(void);      // 处理按钮输入
// // void UpdateLEDs(uint8_t count); // 更新LED状态

// // // 全局变量
// // volatile uint8_t led_count = 1;   // 当前设定同时点亮的LED数量
// // volatile uint8_t current_led = 0; // 当前已点亮的LED数量
// // volatile uint8_t offset = 0;      // 偏移量

// // int main(void) {
// //     Init_GPIO(); // 初始化GPIO

// //     while (1) {
// //         ProcessButtons(); // 持续处理按钮逻辑
// //     }
// // }

// // void Init_GPIO(void) {
// //     // 启用GPIOB和GPIOC时钟
// //     RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

// //     // 配置GPIOB (PB0, PB7, PB14, PB8, PB9, PB10为输出模式)
// //     GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE7_0 | GPIO_MODER_MODE14_0 |
// //                      GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0);
// //     GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT7 | GPIO_OTYPER_OT14 |
// //                        GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9 | GPIO_OTYPER_OT10);
// //     GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR7 |
// //                        GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR8 |
// //                        GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10);
// //     GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD14 |
// //                       GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10);

// //     // 配置GPIOC (PC6和PC12为输入模式)
// //     GPIOC->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE12);
// //     GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD12);
// // }

// // void ProcessButtons(void) {
// //     // 处理按钮1 (PC6) - 设置同时点亮LED数量
// //     if ((GPIOC->IDR & GPIO_IDR_ID6) != 0) {
// //         for (volatile int i = 0; i < 100000; i++); // 简单去抖动
// //         if ((GPIOC->IDR & GPIO_IDR_ID6) != 0) {
// //             led_count++;
// //             if (led_count > 6) led_count = 1; // 循环回到1
// //         }
// //     }

// //     // 处理按钮2 (PC12) - 控制LED点亮/熄灭并添加偏移
// //     if ((GPIOC->IDR & GPIO_IDR_ID12) != 0) {
// //         for (volatile int i = 0; i < 100000; i++); // 简单去抖动
// //         if ((GPIOC->IDR & GPIO_IDR_ID12) != 0) {
// //             offset ++; // 偏移量每次按下按钮2后递增
// //             //UpdateLEDs(led_count); // 根据当前设置数量和偏移更新LED
// //         if(led_count == 6 & current_led == 0){
// //             current_led = 6;//重置状态
// //         }
// //         else if(led_count == 6 & current_led == 6)
// //         {
// //             current_led = 0;//重置状态
// //         }
// //         GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
// //                    GPIO_BSRR_BR8 | GPIO_BSRR_BR9 | GPIO_BSRR_BR10);
// //     //熄灭所所有LED，然后根据需要重亮
// //         if (offset >= (6-current_led))
// //         {
// //             offset = 0;//这是不拐头的，如4灯则0123，1234，2345；offset只有0，1，2
// //         }
// //         for(int  i = offset; i < offset+current_led;  i++)
// //         {
// //             LedOn(int i) //点亮第i个灯,如2灯则01，12，23，34, 45；offset有0,1,2,3,4
// //         }
// //         //UpdateLEDs(current_led); // 更新点亮状态，current_led有1、2、3、4、5、6、0七种状态


// //         }
// //     }
// // }
// // LedOn(int i) //点亮第i个灯
// // {
// // i += 1;
// // Switch(i)
// // {
// // case  1 : GPIOB->BSRR = GPIO_BSRR_BS0;  // 点亮LED1
// // case  2 : GPIOB->BSRR = GPIO_BSRR_BS7;  // 点亮LED2
// // case  3 : GPIOB->BSRR = GPIO_BSRR_BS14;  // 点亮LED3
// // case  4 : GPIOB->BSRR = GPIO_BSRR_BS8;  // 点亮LED4
// // case  5 : GPIOB->BSRR = GPIO_BSRR_BS9;  // 点亮LED5
// // case  6 : GPIOB->BSRR = GPIO_BSRR_BS10;  // 点亮LED6
// // else  return;
// // }
// // }

// // 修改前代码的结尾
// // void UpdateLEDs(uint8_t count) {
// //     // 先关闭所有LED
// //     GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
// //                    GPIO_BSRR_BR8 | GPIO_BSRR_BR9 | GPIO_BSRR_BR10);

// //     // 根据数量和偏移量点亮对应LED
// //     uint8_t leds[6] = {GPIO_BSRR_BS0, GPIO_BSRR_BS7, GPIO_BSRR_BS14, GPIO_BSRR_BS8, GPIO_BSRR_BS9, GPIO_BSRR_BS10};
// //     for (uint8_t i = 0; i < count; i++) {
// //         GPIOB->BSRR = leds[(i + offset) % 6]; // 根据偏移计算点亮位置
// //     }
// // }




// 函数声明(正确一半，45灯有问题无法实现偏移)
#include <stm32f429xx.h>
void Init_GPIO(void);           // 初始化GPIO
void ProcessButtons(void);      // 处理按钮输入
void UpdateLEDs(uint8_t count); // 更新LED状态

volatile uint8_t led_count = 1;  // 当前设定同时点亮的LED数量
volatile uint8_t current_led = 0; // 当前已点亮的LED数量

int main(void) {
    Init_GPIO(); // 初始化GPIO

    while (1) {
        ProcessButtons(); // 循环处理按钮逻辑
    }
}

void Init_GPIO(void) {
    // 启用GPIOB和GPIOC时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    // 配置GPIOB (PB0, PB7, PB14, PB8, PB9, PB10为输出模式)
    GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE7_0 | GPIO_MODER_MODE14_0 |
                     GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0);
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT7 | GPIO_OTYPER_OT14 |
                       GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9 | GPIO_OTYPER_OT10);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR7 |
                       GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR8 |
                       GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10);
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD14 |
                      GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10);

    // 配置GPIOC (PC13和PC12为输入模式)
    GPIOC->MODER &= ~(GPIO_MODER_MODE13 | GPIO_MODER_MODE12);
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13 | GPIO_PUPDR_PUPD12);
}

void ProcessButtons(void) {
    // 处理按钮1 (PC6) - 设置同时点亮LED数量
    if ((GPIOC->IDR & GPIO_IDR_ID6) != 0) {
        for (volatile int i = 0; i < 100000; i++); // 简单去抖动
        if ((GPIOC->IDR & GPIO_IDR_ID6) == 0) {
            led_count++;
            if (led_count > 6) led_count = 1; // 循环回到1
        }
    }

    // 处理按钮2 (PC12) - 控制LED点亮/熄灭
    if ((GPIOC->IDR & GPIO_IDR_ID12) != 0) {
        for (volatile int i = 0; i < 100000; i++); // 简单去抖动
        if ((GPIOC->IDR & GPIO_IDR_ID12) == 0) {
            current_led += led_count;
            if (current_led > 6) {
                current_led = 0; // 重置状态
                GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
                               GPIO_BSRR_BR8 | GPIO_BSRR_BR9 | GPIO_BSRR_BR10); // 熄灭所有LED
            } else {
                UpdateLEDs(current_led); // 更新点亮状态
            }
        }
    }
}

void UpdateLEDs(uint8_t count) {
    // 先关闭所有LED
    GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
                   GPIO_BSRR_BR8 | GPIO_BSRR_BR9 | GPIO_BSRR_BR10);

    // 根据数量点亮对应LED
    if (count >= 1) GPIOB->BSRR = GPIO_BSRR_BS0;  // 点亮LED1
    if (count >= 2) GPIOB->BSRR = GPIO_BSRR_BS7;  // 点亮LED2
    if (count >= 3) GPIOB->BSRR = GPIO_BSRR_BS14; // 点亮LED3
    if (count >= 4) GPIOB->BSRR = GPIO_BSRR_BS8;  // 点亮LED4
    if (count >= 5) GPIOB->BSRR = GPIO_BSRR_BS9;  // 点亮LED5
    if (count >= 6) GPIOB->BSRR = GPIO_BSRR_BS10; // 点亮LED6
}
正确代码结束




