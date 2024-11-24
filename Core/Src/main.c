1#include "stdint.h"

// int main(void) {
//  *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02; //Включение тактирования порта GPIOB
//  *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
//  *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //Настройка на PushPull работу 7-го пина GPIOB (Output Push-Pull)
//  *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; //Настройка скорости работы 7-го пина GPIOB на среднюю
//  *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00; //Отключение PU/PDрезисторов для 7-го пина GPIOB
//  while(1){
//  *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80; //Установка единицы в 7-ой бит регистра ODR
//  }
// }

#include <stdint.h>

// 开启时钟B,C
#define RCC_GPIO_EN                   (*(uint32_t*)(0x40023800UL + 0x30UL))
#define RCC_GPIOB_EN                  0x02UL // 时钟B
#define RCC_GPIOC_EN                  0x04UL // 时钟C

// GPIOB 寄存器
#define GPIOB_MODER                   (*(uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_OTYPER                  (*(uint32_t*)(0x40020400UL + 0x04UL))
#define GPIOB_OSPEEDR                 (*(uint32_t*)(0x40020400UL + 0x08UL))
#define GPIOB_PUPDR                   (*(uint32_t*)(0x40020400UL + 0x0CUL)) 
#define GPIOB_BSRR                    (*(uint32_t*)(0x40020400UL + 0x18UL))

// GPIOC 寄存器
#define GPIOC_IDR                     (*(uint32_t*)(0x40020800UL + 0x10UL))

// 按钮引脚
#define GPIOC_IDR_PIN13               0x002000UL

// LED1 (绿色)
#define GPIOB_MODE_PIN0_OUT           (0x01 << (0 * 2)) // MODER
#define GPIOB_BSRR_PIN0_SET           (0x01) // 点亮
#define GPIOB_BSRR_PIN0_RESET         (0x010000) // 熄灭

// LED2 (蓝色)
#define GPIOB_MODE_PIN7_OUT           (0x01 << (7 * 2)) // MODER
#define GPIOB_BSRR_PIN7_SET           (0x80) // 点亮
#define GPIOB_BSRR_PIN7_RESET         (0x800000) // 熄灭

// LED3 (红色)
#define GPIOB_MODE_PIN14_OUT          (0x01 << (14 * 2)) // MODER
#define GPIOB_BSRR_PIN14_SET          (0x4000) // 点亮
#define GPIOB_BSRR_PIN14_RESET        (0x40000000) // 熄灭

// LED4 (外接LED)
#define GPIOB_MODE_PIN10_OUT          (0x01 << (10 * 2)) // MODER
#define GPIOB_BSRR_PIN10_SET          (0x400) // 点亮
#define GPIOB_BSRR_PIN10_RESET        (0x400000) // 熄灭

#define READ_BIT(REG, BIT) (REG & BIT)
#define SET_BIT(REG, BIT) (REG |= BIT)

void GPIO_Ini(void);

// 全局变量
volatile uint8_t led_state = 0; // 当前打开的 LED 状态
volatile uint8_t button_press_count = 0; // 按钮按下次数

int main(void) {
    GPIO_Ini();

    // 在程序开始时确保所有LED都熄灭
    GPIOB_BSRR |= (GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | GPIOB_BSRR_PIN10_RESET);

    while (1) {
        // 按钮状态检测
        if (!(GPIOC_IDR & GPIOC_IDR_PIN13)) { // 检查按钮是否按下
            // 简单的去抖动处理
            for (volatile int i = 0; i < 100000; i++); // 延时

            if (!(GPIOC_IDR & GPIOC_IDR_PIN13)) { // 再次确认按钮按下
                button_press_count++;

                if (button_press_count % 6 == 0) {
                    // 每按六次关闭所有LED并重置状态
                    GPIOB_BSRR |= (GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | GPIOB_BSRR_PIN10_RESET);
                    led_state = 0; // 重置状态
                } else {
                    // 打开下一个 LED
                    switch (led_state) {
                        case 0:
                            GPIOB_BSRR |= GPIOB_BSRR_PIN0_SET; // 点亮 LED1
                            break;
                        case 1:
                            GPIOB_BSRR |= GPIOB_BSRR_PIN7_SET; // 点亮 LED2
                            break;
                        case 2:
                            GPIOB_BSRR |= GPIOB_BSRR_PIN14_SET; // 点亮 LED3
                            break;
                        case 3:
                            GPIOB_BSRR |= GPIOB_BSRR_PIN10_SET; // 点亮 LED4
                            break;
                        default:
                            // 所有 LED 都已点亮，关闭所有 LED
                            GPIOB_BSRR |= (GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET | GPIOB_BSRR_PIN10_RESET);
                            led_state = 0; // 重置状态
                            break;
                    }
                    led_state++;
                }

                // 等待按钮释放
                while (!(GPIOC_IDR & GPIOC_IDR_PIN13));
            }
        }
    }
}

void GPIO_Ini(void) {
    // 使能 GPIOB 和 GPIOC 时钟
    RCC_GPIO_EN |= (RCC_GPIOB_EN | RCC_GPIOC_EN);

    // 配置 GPIOB 引脚为输出模式
    GPIOB_MODER |= (GPIOB_MODE_PIN0_OUT | GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT | GPIOB_MODE_PIN10_OUT); // 将指定引脚配置为输出模式

    // 其他配置（如果需要）
    GPIOB_OTYPER &= ~(0x0); // 全部推挽输出
    GPIOB_OSPEEDR |= (0x0); // 全部中速
    GPIOB_PUPDR &= ~(0x0); // 无上拉下拉

    // 配置 GPIOC 引脚为输入模式
    GPIOB_MODER &= ~(3 << (13 * 2)); // 清除 GPIOC 引脚的模式位，设置为输入模式
}
