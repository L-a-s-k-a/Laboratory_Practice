#include "init.h"
#include <stdint.h>

// ==================== 全局变量 ====================
uint8_t led_states[3] = {0}; // 记录每个 LED 的当前状态 (0=关闭, 1=打开) 对应 LED1 (PB0), LED2 (PB7), LED3 (PB14)
uint8_t button1_mode = 0;    // 按钮1的功能模式 (0=开灯, 1=关灯)
uint8_t direction = 0;       // 操作方向 (0=正向, 1=反向)
uint8_t button1_pressed = 0;
uint8_t button2_pressed = 0;

// ==================== 函数原型 ====================
void update_leds(void);
void delay(volatile uint32_t s);

int main(void)
{
    // 初始化 GPIO
    GPIO_Ini_1(); // 初始化 LED 引脚
    GPIO_Ini_2(); // 初始化按钮引脚

    // 根据初始模式设置方向
    if (button1_mode == 0)
    {
        direction = 0; // 开灯时为正向
    }
    else
    {
        direction = 1; // 关灯时为反向
    }

    while (1)
    {
        // 读取按钮1 (PC13) 和按钮2 (PC6) 的状态
        uint32_t button1_state = GPIOC_IDR & GPIOC_IDR_PIN13;
        uint32_t button2_state = GPIOC_IDR & GPIOC_IDR_PIN6;

        // ==================== 按钮1 逻辑 ====================
        if (!button1_state) // 低电平有效
        {
            if (!button1_pressed)
            {
                button1_pressed = 1;

                uint8_t all_done = 1;
                if (direction == 0)
                {
                    // 正向操作
                    for (int i = 2; i >= 0; i--)
                    {
                        if (led_states[i] != button1_mode)
                        {
                            led_states[i] = button1_mode;
                            all_done = 0;
                            break;
                        }
                    }
                }
                else
                {
                    // 反向操作
                    for (int i = 0; i < 3; i++)
                    {
                        if (led_states[i] != button1_mode)
                        {
                            led_states[i] = button1_mode;
                            all_done = 0;
                            break;
                        }
                    }
                }

                if (all_done)
                {
                    // 所有 LED 已达到目标状态，不再切换方向
                    // 如果需要，可以在这里添加其他逻辑
                }

                update_leds();
            }
        }
        else
        {
            button1_pressed = 0;
        }

        // ==================== 按钮2 逻辑 ====================
        if (!button2_state) // 低电平有效
        {
            if (!button2_pressed)
            {
                button2_pressed = 1;
                button1_mode = !button1_mode; // 切换模式

                // 根据切换后的模式设置方向
                if (button1_mode == 0)
                {
                    direction = 0; // 开灯时为正向
                }
                else
                {
                    direction = 1; // 关灯时为反向
                }
            }
        }
        else
        {
            button2_pressed = 0;
        }

        // ==================== 防抖延时 ====================
        delay(100000); // 根据需要调整延时时间以实现适当的防抖
    }
}

// ==================== 更新 LED 状态函数 ====================
void update_leds(void)
{
    // 关闭所有 LED
    GPIOB_BSRR = GPIOB_BSRR_PIN0_RESET | GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET;

    // 根据 led_states 数组打开相应的 LED
    if (led_states[0])
        GPIOB_BSRR = GPIOB_BSRR_PIN0_SET;
    if (led_states[1])
        GPIOB_BSRR = GPIOB_BSRR_PIN7_SET;
    if (led_states[2])
        GPIOB_BSRR = GPIOB_BSRR_PIN14_SET;
}

// ==================== 延时函数 ====================
void delay(volatile uint32_t s)
{
    while (s--)
        ;
}
