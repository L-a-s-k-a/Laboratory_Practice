#include <stdint.h>
#include "init.h"



// uint8_t buttonState = 1;
// uint8_t lastButtonState = 0;
// volatile uint8_t counter = 0;
// uint8_t led1_state = 0;
// uint8_t led2_state = 0;
// uint8_t led3_state = 0;
// uint8_t led4_state = 0;
void delay(void);
void UpdateLEDs(uint8_t count);   // 更新LED状态
volatile uint8_t led_count = 0;   // 当前设定同时点亮的LED数量
volatile uint8_t current_led = 0; // 当前已点亮的LED数量



int main(void)
{

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOBEN;

    // THE LED SETTINGS
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE8_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE9_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE10_0);

    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_8);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_9);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_10);

    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR9_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR10_0);

    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR8_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR9_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR10_0);

    // THE BUTTON SETTINGS PC13
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE13);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD13_1);

    while (1)
    {

        // 处理按钮1 (PC6) - 设置同时点亮LED数量
        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)) != 0) // Detect whether PC13 is pressed
        {
            delay();                                          // Stabilization
            if ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)) != 0) //  PC13 is pressed
            {
                led_count++;
                if (led_count > 6)
                    led_count = 0; // 循环回到1
                while ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)) != 0)
                    ; // Wait  button free
            }
        }

        // 处理按钮2 (PC12) - 控制LED点亮/熄灭
        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_6)) != 0) // Detect whether PC13 is pressed
        {
            delay();                                          // Stabilization
            if ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_6)) != 0) //  PC13 is pressed
            {
                current_led += led_count;
                if (current_led > 6)
                {
                    led_count = 0; // 循环回到1
                    current_led = 0; // 重置状态
                    GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14);
                    GPIOB->BSRR = (GPIO_BSRR_BR8 | GPIO_BSRR_BR9 | GPIO_BSRR_BR10); // 熄灭所有LED
                }
                else
                {
                    UpdateLEDs(current_led); // 更新点亮状态
                }
                while ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_6)) != 0)
                    ; // Wait  button free
            }
        }
    }

    return 0;
}

void UpdateLEDs(uint8_t count)
{
    // 先关闭所有LED
    GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14);
    GPIOB->BSRR = (GPIO_BSRR_BR8 | GPIO_BSRR_BR9 | GPIO_BSRR_BR10);

    // 根据数量点亮对应LED
    if (count >= 1)
        GPIOB->BSRR = GPIO_BSRR_BS0; // 点亮LED1
    if (count >= 2)
        GPIOB->BSRR = GPIO_BSRR_BS7; // 点亮LED2
    if (count >= 3)
        GPIOB->BSRR = GPIO_BSRR_BS14; // 点亮LED3
    if (count >= 4)
        GPIOB->BSRR = GPIO_BSRR_BS8; // 点亮LED4
    if (count >= 5)
        GPIOB->BSRR = GPIO_BSRR_BS9; // 点亮LED5
    if (count >= 6)
        GPIOB->BSRR = GPIO_BSRR_BS10; // 点亮LED6
}

void delay(void)
{
    for (volatile int i = 0; i < 100000; i++)
    {
        __asm__("nop");
    }
}
