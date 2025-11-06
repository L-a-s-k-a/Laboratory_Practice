#include <stdint.h> 
#include <init.h> 

// // 函数声明
// void Init_GPIO(void);           // 初始化GPIO
// void ProcessButtons(void);      // 处理按钮输入
// void UpdateLEDs(uint8_t count); // 更新LED状态

// // 全局变量
// volatile uint8_t led_count = 1;   // 当前设定同时点亮的LED数量
// volatile uint8_t current_led = 0; // 当前已点亮的LED数量
// volatile uint8_t offset = 0;      // 偏移量

// int main(void) {
//     Init_GPIO(); // 初始化GPIO

//     while (1) {
//         ProcessButtons(); // 持续处理按钮逻辑
//     }
// }

// void Init_GPIO(void) {
//     // 启用GPIOB和GPIOC时钟
//     RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

//     // 配置GPIOB (PB0, PB7, PB14, PB8, PB9, PB10为输出模式)
//     GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE7_0 | GPIO_MODER_MODE14_0 |
//                      GPIO_MODER_MODE8_0 | GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0);
//     GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT7 | GPIO_OTYPER_OT14 |
//                        GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9 | GPIO_OTYPER_OT10);
//     GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR7 |
//                        GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR8 |
//                        GPIO_OSPEEDER_OSPEEDR9 | GPIO_OSPEEDER_OSPEEDR10);
//     GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD14 |
//                       GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10);

//     // 配置GPIOC (PC6和PC12为输入模式)
//     GPIOC->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE12);
//     GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD12);
// }

// void ProcessButtons(void) {
//     // 处理按钮1 (PC6) - 设置同时点亮LED数量
//     if ((GPIOC->IDR & GPIO_IDR_ID6) != 0) {
//         for (volatile int i = 0; i < 100000; i++); // 简单去抖动
//         if ((GPIOC->IDR & GPIO_IDR_ID6) != 0) {
//             led_count++;
//             if (led_count > 6) led_count = 1; // 循环回到1
//         }
//     }

//     // 处理按钮2 (PC12) - 控制LED点亮/熄灭并添加偏移
//     if ((GPIOC->IDR & GPIO_IDR_ID12) != 0) {
//         for (volatile int i = 0; i < 100000; i++); // 简单去抖动
//         if ((GPIOC->IDR & GPIO_IDR_ID12) != 0) {
//             offset ++; // 偏移量每次按下按钮2后递增
//             //UpdateLEDs(led_count); // 根据当前设置数量和偏移更新LED
//         if(led_count == 6 & current_led == 0){
//             current_led = 6;//重置状态
//         }
//         else if(led_count == 6 & current_led == 6)
//         {
//             current_led = 0;//重置状态
//         }
//         GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
//                    GPIO_BSRR_BR8 | GPIO_BSRR_BR9 | GPIO_BSRR_BR10);
//     //熄灭所所有LED，然后根据需要重亮
//         if (offset >= (6-current_led))
//         {
//             offset = 0;//这是不拐头的，如4灯则0123，1234，2345；offset只有0，1，2
//         }
//         for(int  i = offset; i < offset+current_led;  i++)
//         {
//             LedOn(int i) //点亮第i个灯,如2灯则01，12，23，34, 45；offset有0,1,2,3,4
//         }
//         //UpdateLEDs(current_led); // 更新点亮状态，current_led有1、2、3、4、5、6、0七种状态


//         }
//     }
// }
// LedOn(int i) //点亮第i个灯
// {
// i += 1;
// Switch(i)
// {
// case  1 : GPIOB->BSRR = GPIO_BSRR_BS0;  // 点亮LED1
// case  2 : GPIOB->BSRR = GPIO_BSRR_BS7;  // 点亮LED2
// case  3 : GPIOB->BSRR = GPIO_BSRR_BS14;  // 点亮LED3
// case  4 : GPIOB->BSRR = GPIO_BSRR_BS8;  // 点亮LED4
// case  5 : GPIOB->BSRR = GPIO_BSRR_BS9;  // 点亮LED5
// case  6 : GPIOB->BSRR = GPIO_BSRR_BS10;  // 点亮LED6
// else  return;
// }
// }

// 修改前代码的结尾
// void UpdateLEDs(uint8_t count) {
//     // 先关闭所有LED
//     GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
//                    GPIO_BSRR_BR8 | GPIO_BSRR_BR9 | GPIO_BSRR_BR10);

//     // 根据数量和偏移量点亮对应LED
//     uint8_t leds[6] = {GPIO_BSRR_BS0, GPIO_BSRR_BS7, GPIO_BSRR_BS14, GPIO_BSRR_BS8, GPIO_BSRR_BS9, GPIO_BSRR_BS10};
//     for (uint8_t i = 0; i < count; i++) {
//         GPIOB->BSRR = leds[(i + offset) % 6]; // 根据偏移计算点亮位置
//     }
// }




// 函数声明(正确一半，45灯有问题无法实现偏移)
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




// // 函数声明
// void Init_GPIO(void);           // 初始化GPIO
// void ProcessButtons(void);      // 处理按钮输入
// void UpdateLEDs(uint8_t count); // 更新LED状态

// // 全局变量
// uint8_t led_count = 1;   // 当前设定同时点亮的LED数量
// uint8_t current_led = 0; // 当前已点亮的LED数量

// int main(void) {
//     Init_GPIO(); // 初始化GPIO

//     while (1) {
//         if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID13) != 0) {
//             for (volatile int i = 0; i < 100000; i++); 
//             led_count++;
//             if (led_count > 6) led_count = 1; // 循环回到1
//     }
//     if (led_count >= 1) GPIOB->BSRR = GPIO_BSRR_BS14;  // 点亮LED1
//     if (led_count >= 2) GPIOB->BSRR = GPIO_BSRR_BS7;  // 点亮LED2
//     if (led_count >= 3) GPIOB->BSRR = GPIO_BSRR_BS0;  // 点亮LED3
//     if (led_count >= 4) GPIOB->BSRR = GPIO_BSRR_BS3;  // 点亮LED4
//     if (led_count >= 5) GPIOB->BSRR = GPIO_BSRR_BS5;  // 点亮LED5
//     if (led_count >= 6) GPIOB->BSRR = GPIO_BSRR_BS4; // 点亮LED6

// }
// }



// void Init_GPIO(void) {
//     // 开启GPIOB和GPIOC时钟
//     RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

//     // 配置GPIOB输出引脚（PB2, PB6, PB1, PB3, PB5, PB12）
//     GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE7_0 | GPIO_MODER_MODE14_0 |
//                      GPIO_MODER_MODE3_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE4_0);
//     GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT7 | GPIO_OTYPER_OT14 |
//                        GPIO_OTYPER_OT3 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT4);
//     GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR7_1 |
//                        GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR3_1 |
//                        GPIO_OSPEEDER_OSPEEDR5_1 | GPIO_OSPEEDER_OSPEEDR4_1);
//     GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD14 |
//                       GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD4);

//     // 配置GPIOC输入引脚（PC13, PC12）
//     GPIOC->MODER &= ~(GPIO_MODER_MODE13 | GPIO_MODER_MODE12);
//     // GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13 | GPIO_PUPDR_PUPD12);
// }

// void ProcessButtons(void) {
//     // 检测按钮1 (PC13) - 设置同时点亮的LED数量
//     if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID13) != 0) {
//         for (volatile int i = 0; i < 100000; i++); // 简单去抖动
//         if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID13) != 0) {
//             led_count++;
//             if (led_count > 6) led_count = 1; // 循环回到1
//         }
//     }

//     // // 检测按钮2 (PC12) - 控制LED点亮/熄灭
//     // if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID12) != 0) {
//     //     for (volatile int i = 0; i < 100000; i++); // 简单去抖动
//     //     if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID12) != 0) {
//     //         current_led += led_count;
//     //         if (current_led >= 6) {
//     //             current_led = 0; // 重置状态
//     //             GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
//     //                            GPIO_BSRR_BR3 | GPIO_BSRR_BR5 | GPIO_BSRR_BR4); // 熄灭所有LED
//     //         } else {
//     //             UpdateLEDs(current_led); // 更新点亮状态
//     //         }
//     //     }
//     // }
// }

// void UpdateLEDs(uint8_t count) {
//     // 熄灭所有LED
//     GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
//                     GPIO_BSRR_BR3 | GPIO_BSRR_BR5 | GPIO_BSRR_BR4);

//     // 点亮指定数量的LED
//     if (count >= 1) GPIOB->BSRR = GPIO_BSRR_BS14;  // 点亮LED1
//     if (count >= 2) GPIOB->BSRR = GPIO_BSRR_BS7;  // 点亮LED2
//     if (count >= 3) GPIOB->BSRR = GPIO_BSRR_BS0;  // 点亮LED3
//     if (count >= 4) GPIOB->BSRR = GPIO_BSRR_BS3;  // 点亮LED4
//     if (count >= 5) GPIOB->BSRR = GPIO_BSRR_BS5;  // 点亮LED5
//     if (count >= 6) GPIOB->BSRR = GPIO_BSRR_BS4; // 点亮LED6
// }






// int main(void) { 

//       SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN |  RCC_AHB1ENR_GPIOCEN);
//     // SET_BIT(RCC_GPIO_EN,RCC_GPIOB_EN | RCC_GPIOC_EN | RCC_GPIOC_EN );
//    // *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x06; //Включение тактирования портов GPIOB и GPIOC 
//      SET_BIT(GPIOB_MODER,GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT | GPIOB_MODE_PIN0_OUT);
//     //*(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
//       SET_BIT(GPIOB_OTYPER,GPIOB_OTYPE_PIN7_PP | GPIOB_OTYPE_PIN14_PP | GPIOB_OTYPE_PIN0_PP);
//     //*(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //Настройка на PushPull работу 7-го пина GPIOB (Output Push-Pull) 
//     SET_BIT(GPIOB_OSPEEDR, GPIOB_OSPEED_PIN7_MID | GPIOB_OSPEED_PIN14_MID | GPIOB_OSPEED_PIN0_MID);
//     //*(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; //Настройка скорости работы 7-го пина GPIOB на среднюю 
//     SET_BIT(GPIOB_PUPDR,GPIOB_PUPDR_PIN7_NOPUPD | GPIOB_PUPDR_PIN14_NOPUPD | GPIOB_PUPDR_PIN0_NOPUPD);
    
//     //*(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00; //Отключение PU/PD резисторов для 7-го пина GPIOB 
//     while(1){ 
//         if((READ_BIT(GPIOC_IDR ,GPIOC_IDR_PIN13  ) != 0)){ 
//             SET_BIT(GPIOB_BSRR ,GPIOB_BSRR_PIN7_SET | GPIOB_BSRR_PIN14_SET | GPIOB_BSRR_PIN0_SET);
//            // *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80; //Установка единицы в 7-ой бит регистра ODR 
//         } 
//         else{ 
//              SET_BIT(GPIOB_BSRR ,GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | GPIOB_BSRR_PIN0_RESET);
//             //*(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000; //Установка нуля в 7-ой бит регистра ODR 
//         } 
//     } 
// } 
