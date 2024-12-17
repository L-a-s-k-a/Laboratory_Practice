#include <stdint.h> 
#include <init.h>


#include <stdint.h>
#include <stdbool.h>

// GPIO 初始化
void GPIO_Ini(void);
void ToggleLED(uint8_t led_index, uint8_t state);
void TurnOffAllLEDs(void);
void TurnOnAllLEDs(void);

volatile uint8_t led_states[4] = {0, 0, 0, 0}; // 记录每个 LED 的状态
volatile uint8_t button_count = 0;            // 按钮按下次数
volatile uint8_t button_flag = 0;

int main(void) {
    uint8_t button_pressed = 0; // 按钮状态
    GPIO_Ini();                 // 初始化 GPIO

    while (1) {
        // 更新按钮状态
        button_flag = READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) ? 1 : 0;

        // 检测按钮是否按下
        if (READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) != 0) { // 按钮按下
            for (volatile int i = 0; i < 100000; i++); // 简单延时，用于消抖
            if (READ_BIT(*(volatile uint32_t *)GPIOC_IDR, GPIOC_IDR_PIN13) == 0 && button_pressed == 0) {
                button_pressed = 1; // 标记按钮已经按下
                button_count++;     // 增加按钮按下次数

                // 根据按钮按下次数控制灯
                if (button_count >= 1 && button_count <= 4) {
                    // 点亮指定数量的 LED
                    for (uint8_t i = 0; i < 4; i++) {
                        if (i < button_count) {
                            ToggleLED(i + 1, 1); // 打开 LED
                            led_states[i] = 1;  // 更新状态
                        } else {
                            ToggleLED(i + 1, 0); // 关闭 LED
                            led_states[i] = 0;  // 更新状态
                        }
                    }
                } else if (button_count == 5) {
                    // 关闭所有 LED
                    TurnOffAllLEDs();
                    for (uint8_t i = 0; i < 4; i++) {
                        led_states[i] = 0; // 更新状态
                    }
                } else if (button_count == 6) {
                    // 点亮所有 LED
                    TurnOnAllLEDs();
                    for (uint8_t i = 0; i < 4; i++) {
                        led_states[i] = 1; // 更新状态
                    }
                } else if (button_count > 6) {
                    // 重置逻辑并重新开始
                    TurnOffAllLEDs();
                    for (uint8_t i = 0; i < 4; i++) {
                        led_states[i] = 0; // 更新状态
                    }
                    button_count = 0; // 重置计数
                }
            }
        } else {
            button_pressed = 0; // 按钮释放
        }
    }
}

// GPIO 初始化
void GPIO_Ini(void) {
    SET_BIT(*(volatile uint32_t *)RCC_GPIO_EN, RCC_GPIOB_EN | RCC_GPIOC_EN);
    *(volatile uint32_t *)GPIOB_MODER |= GPIOB_MODE_PIN0_OUT | GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT | GPIOB_MODE_PIN10_OUT;
}

// 切换 LED 状态
void ToggleLED(uint8_t led_index, uint8_t state) {
    switch (led_index) {
        case 1:
            *(volatile uint32_t *)GPIOB_BSRR = state ? GPIOB_BSRR_PIN0_SET : GPIOB_BSRR_PIN0_RESET;
            break;
        case 2:
            *(volatile uint32_t *)GPIOB_BSRR = state ? GPIOB_BSRR_PIN7_SET : GPIOB_BSRR_PIN7_RESET;
            break;
        case 3:
            *(volatile uint32_t *)GPIOB_BSRR = state ? GPIOB_BSRR_PIN14_SET : GPIOB_BSRR_PIN14_RESET;
            break;
        case 4:
            *(volatile uint32_t *)GPIOB_BSRR = state ? GPIOB_BSRR_PIN10_SET : GPIOB_BSRR_PIN10_RESET;
            break;
    }
}

// 关闭所有 LED
void TurnOffAllLEDs(void) {
    *(volatile uint32_t *)GPIOB_BSRR = GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | GPIOB_BSRR_PIN10_RESET;
}

// 点亮所有 LED
void TurnOnAllLEDs(void) {
    *(volatile uint32_t *)GPIOB_BSRR = GPIOB_BSRR_PIN0_SET | GPIOB_BSRR_PIN7_SET | GPIOB_BSRR_PIN14_SET | GPIOB_BSRR_PIN10_SET;
}


// int main(void) { 

//      SET_BIT(RCC_GPIO_EN,RCC_GPIOB_EN | RCC_GPIOC_EN | RCC_GPIOC_EN);
//    // *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x06; //Включение тактирования портов GPIOB и GPIOC 
//      SET_BIT(GPIOB_MODER,GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT);
//     //*(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
//       SET_BIT(GPIOB_OTYPER,GPIOB_OTYPE_PIN7_PP | GPIOB_OTYPE_PIN14_PP );
//     //*(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //Настройка на PushPull работу 7-го пина GPIOB (Output Push-Pull) 
//     SET_BIT(GPIOB_OSPEEDR, GPIOB_OSPEED_PIN7_MID | GPIOB_OSPEED_PIN14_MID);
//     //*(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; //Настройка скорости работы 7-го пина GPIOB на среднюю 
//     SET_BIT(GPIOB_PUPDR,GPIOB_PUPDR_PIN7_NOPUPD | GPIOB_PUPDR_PIN14_NOPUPD);
    
//     //*(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00; //Отключение PU/PD резисторов для 7-го пина GPIOB 
//     while(1){ 
//         if((READ_BIT(GPIOC_IDR ,GPIOC_IDR_PIN13  ) != 0)){ 
//             SET_BIT(GPIOB_BSRR ,GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET);
//            // *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80; //Установка единицы в 7-ой бит регистра ODR 
//         } 
//         else{ 
//              SET_BIT(GPIOB_BSRR ,GPIOB_BSRR_PIN7_SET | GPIOB_BSRR_PIN14_SET);
//             //*(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000; //Установка нуля в 7-ой бит регистра ODR 
//         } 
//     } 
// } 


// int main(void) {
//     *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02; //Включение тактирования портов GPIOB и GPIOC
//     *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
//     *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
//     *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; //Настройка скорости работы 7-го пина GPIOB на среднюю
//     *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00; //Отключение PU/PDрезисторов для 7-го пина GPIOB
//     while(1){
//         if((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x2000UL) != 0){
//             *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80; //Установка единицы в 7-ой бит регистра ODR
//             }
//             else{
//                 *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000; //Установка нуля в 7-ой бит регистра ODR
//                 }
//             }
// }



