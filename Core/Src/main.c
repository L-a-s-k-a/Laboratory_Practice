#include "init.h"

// 简单的软件延迟函数
void delay(volatile uint32_t count)
{
    while(count--);
}

int main(void)
{
    // 初始化 GPIO
    GPIO_Ini_1();
    GPIO_Ini_2();

    // 定义 LED 引脚数组
    const uint32_t led_pins[] = {0, 7, 14};
    const int num_leds = sizeof(led_pins) / sizeof(led_pins[0]);

    // 定义按钮引脚
    const uint32_t button1_pin = GPIOC_IDR_PIN6;  // 按钮1连接到 PC6
    const uint32_t button2_pin = GPIOC_IDR_PIN13; // 按钮2连接到 PC13

    // 状态变量
    int mode = 1; // 1: 开启 LED, 0: 关闭 LED
    int button1_prev = 1; // 上一次按钮1状态（假设上拉）
    int button2_prev = 1; // 上一次按钮2状态
    int process_running = 0; // 进程是否正在运行

    while(1)
    {
        // 读取按钮1状态
        int button1_current = READ_BIT(GPIOC_IDR, button1_pin) ? 1 : 0;
        // 检测按钮1按下（假设按钮按下时电平为低）
        if(button1_prev == 1 && button1_current == 0)
        {
            // 按钮1被按下，切换模式
            mode = !mode;
            // 等待按钮释放
            while(READ_BIT(GPIOC_IDR, button1_pin) == 0);
        }
        button1_prev = button1_current;

        // 读取按钮2状态
        int button2_current = READ_BIT(GPIOC_IDR, button2_pin) ? 1 : 0;
        // 检测按钮2按下（假设按钮按下时电平为低）
        if(button2_prev == 1 && button2_current == 0 && !process_running)
        {
            // 按钮2被按下，启动 LED 处理过程
            process_running = 1;

            for(int i = 0; i < num_leds; i++)
            {
                if(mode)
                {
                    // 开启 LED
                    if(led_pins[i] == 0)
                        GPIOB_BSRR = GPIOB_BSRR_PIN0_SET;
                    else if(led_pins[i] == 7)
                        GPIOB_BSRR = GPIOB_BSRR_PIN7_SET;
                    else if(led_pins[i] == 14)
                        GPIOB_BSRR = GPIOB_BSRR_PIN14_SET;
                }
                else
                {
                    // 关闭 LED
                    if(led_pins[i] == 0)
                        GPIOB_BSRR = GPIOB_BSRR_PIN0_RESET;
                    else if(led_pins[i] == 7)
                        GPIOB_BSRR = GPIOB_BSRR_PIN7_RESET;
                    else if(led_pins[i] == 14)
                        GPIOB_BSRR = GPIOB_BSRR_PIN14_RESET;
                }

                delay(1000000); // 简单延迟
            }

            process_running = 0;
        }
        button2_prev = button2_current;
    }

    return 0;
}
