#include "../Inc/init.h"

static uint8_t button_press_count = 0; //按钮监测变量
static uint8_t debounce_flag = 0; //去抖动处理标志

uint8_t LB1 = 0;
uint8_t LB2 = 0;
uint8_t LB3 = 0;
uint8_t LB4 = 0;

int main(void)
{
    GPIO_Init_Memory();
    GPIO_Init_CMSIS();

    // 初始状态：所有LED关闭
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);   // PB0 关闭
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);   // PB7 关闭  
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);  // PB14 关闭
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);  // PB10 关闭

    while(1)
    {
        // 检测按钮按下（GPIOC ID13）
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_ID13))
        {
            // 去抖动处理 - 确保每次按下只计数一次
            if(!debounce_flag)
            {
                debounce_flag = 1;
                
                // 增加按钮按下计数
                button_press_count++;
                if(button_press_count > 6)
                {
                    button_press_count = 1;
                }
                
                // 根据计数控制LED - 使用if判断
                if(button_press_count == 1)
                {
                    // 第一个灯亮（PB0）
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);   // PB7关闭
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);  // PB14关闭
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);  // PB10关闭
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);   // PB0亮
                    LB1 = 1;
                    LB2 = 0;
                    LB3 = 0;
                    LB4 = 0;
                }
                else if(button_press_count == 2)
                {
                    // 前两个灯亮（PB0, PB7）
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);   // PB0亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);   // PB7亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);  // PB14关闭
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);  // PB10关闭
                    LB1 = 1;
                    LB2 = 1;
                    LB3 = 0;
                    LB4 = 0;
                }
                else if(button_press_count == 3)
                {
                    // 前三个灯亮（PB0, PB7, PB14）
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);   // PB0亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);   // PB7亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);  // PB14亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);  // PB10关闭
                    LB1 = 1;
                    LB2 = 1;
                    LB3 = 1;
                    LB4 = 0;
                }
                else if(button_press_count == 4)
                {
                    // 四个灯全亮（PB0, PB7, PB14, PB10）
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);   // PB0亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);   // PB7亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);  // PB14亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS10);  // PB10亮
                    LB1 = 1;
                    LB2 = 1;
                    LB3 = 1;
                    LB4 = 1;
                }
                else if(button_press_count == 5)
                {
                    // 所有LED关闭
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);   // PB0关闭
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);   // PB7关闭
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);  // PB14关闭
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);  // PB10关闭
                    LB1 = 0;
                    LB2 = 0;
                    LB3 = 0;
                    LB4 = 0;
                }
                else if(button_press_count == 6)
                {
                    // 所有LED打开
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);   // PB0亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);   // PB7亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);  // PB14亮
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS10);  // PB10亮
                    LB1 = 1;
                    LB2 = 1;
                    LB3 = 1;
                    LB4 = 1;
                }
                
                // 简单延时，防止过于快速的处理
                for(volatile uint32_t i = 0; i < 100000; i++);
            }
        }
        else
        {
            // 按钮释放，重置去抖动标志
            debounce_flag = 0;
        }
        
        // 添加小延时，避免过于频繁的检测
        for(volatile uint32_t i = 0; i < 1000; i++);
    }
}