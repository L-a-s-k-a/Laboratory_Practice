#include <init.h>

// void GPIO_1(void){

//       SET_BIT(RCC_GPIO_EN,RCC_GPIOB_EN | RCC_GPIOC_EN | RCC_GPIOC_EN);
//    // *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x06; //Включение тактирования портов GPIOB и GPIOC 
//      SET_BIT(GPIOB_MODER,GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT);
//     //*(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
//       SET_BIT(GPIOB_OTYPER,GPIOB_OTYPE_PIN7_PP | GPIOB_OTYPE_PIN14_PP );
//     //*(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //Настройка на PushPull работу 7-го пина GPIOB (Output Push-Pull) 
//     SET_BIT(GPIOB_OSPEEDR, GPIOB_OSPEED_PIN7_MID | GPIOB_OSPEED_PIN14_MID);
//     //*(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; //Настройка скорости работы 7-го пина GPIOB на среднюю 
//     SET_BIT(GPIOB_PUPDR,GPIOB_PUPDR_PIN7_NOPUPD | GPIOB_PUPDR_PIN14_NOPUPD);
    
//     //*(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00; //Отключение PU/PD резисторов для 7-го пина GPIOB 
// }

void Init_GPIO(void) {
    // 开启GPIOB和GPIOC时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    // 配置GPIOB输出引脚（PB2, PB6, PB1, PB3, PB5, PB12）
    GPIOB->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE7_0 | GPIO_MODER_MODE14_0 |
                     GPIO_MODER_MODE3_0 | GPIO_MODER_MODE5_0 | GPIO_MODER_MODE4_0);
    // GPIOB->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT1 |
    //                    GPIO_OTYPER_OT3 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT12);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR0_1 | GPIO_OSPEEDER_OSPEEDR7_1 |
                       GPIO_OSPEEDER_OSPEEDR14_1 | GPIO_OSPEEDER_OSPEEDR3_1 |
                       GPIO_OSPEEDER_OSPEEDR15_1 | GPIO_OSPEEDER_OSPEEDR4_1);
    // GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD1 |
    //                   GPIO_PUPDR_PUPD3 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD12);

    // 配置GPIOC输入引脚（PC13, PC12）
    GPIOC->MODER &= ~(GPIO_MODER_MODE13 | GPIO_MODER_MODE12);
    // GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13 | GPIO_PUPDR_PUPD12);
}

void ProcessButtons(void) {
    // 检测按钮1 (PC13) - 设置同时点亮的LED数量
    if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID13) != 0) {
        for (volatile int i = 0; i < 100000; i++); // 简单去抖动
        if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID13) != 0) {
            led_count++;
            if (led_count > 6) led_count = 1; // 循环回到1
        }
    }

    // 检测按钮2 (PC12) - 控制LED点亮/熄灭
    if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID12) != 0) {
        for (volatile int i = 0; i < 100000; i++); // 简单去抖动
        if (READ_BIT(GPIOC->IDR,GPIO_IDR_ID12) != 0) {
            current_led += led_count;
            if (current_led >= 6) {
                current_led = 0; // 重置状态
                GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
                               GPIO_BSRR_BR3 | GPIO_BSRR_BR5 | GPIO_BSRR_BR4); // 熄灭所有LED
            } else {
                UpdateLEDs(current_led); // 更新点亮状态
            }
        }
    }
}

void UpdateLEDs(uint8_t count) {
    // 熄灭所有LED
    GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14 |
                    GPIO_BSRR_BR3 | GPIO_BSRR_BR5 | GPIO_BSRR_BR4);

    // 点亮指定数量的LED
    if (count >= 1) GPIOB->BSRR = GPIO_BSRR_BS14;  // 点亮LED1
    if (count >= 2) GPIOB->BSRR = GPIO_BSRR_BS7;  // 点亮LED2
    if (count >= 3) GPIOB->BSRR = GPIO_BSRR_BS0;  // 点亮LED3
    if (count >= 4) GPIOB->BSRR = GPIO_BSRR_BS3;  // 点亮LED4
    if (count >= 5) GPIOB->BSRR = GPIO_BSRR_BS5;  // 点亮LED5
    if (count >= 6) GPIOB->BSRR = GPIO_BSRR_BS4; // 点亮LED6
}
