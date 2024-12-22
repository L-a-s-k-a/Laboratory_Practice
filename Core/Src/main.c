#include "init.h"

int main(void) 
{
    GPIO_Ini();
    uint8_t ledIndex = 0; // 当前点亮的LED索引
    uint8_t buttonPressCount = 0; // 按钮按下计数

    while(1)
    {
        if(READ_GPIO_C13 == 0) // 检测按钮是否按下
        {
            for (volatile int i = 0; i < 100000; i++); // 简单去抖动
            while (READ_GPIO_C13 == 0); // 等待按钮释放

            buttonPressCount++; // 增加按钮按下计数

            if (buttonPressCount <= 4) {
                // 点亮下一个LED
                if (ledIndex < 4) {
                    SET_GPIO_B(ledIndex == 0 ? 0 : (ledIndex == 1 ? 7 : (ledIndex == 2 ? 14 : 15))); // PB0, PB7, PB14, PB15
                    ledIndex++;
                }
            } 
            else if (buttonPressCount == 5) {
                // 熄灭所有LED并重置
                RESET_GPIO_B(0);   // PB0
                RESET_GPIO_B(7);   // PB7
                RESET_GPIO_B(14);  // PB14
                RESET_GPIO_B(15);  // PB15 (外接LED)
                ledIndex = 0; // 重置LED索引
            } 
            else if (buttonPressCount == 6) {
                // 点亮所有LED
                SET_GPIO_B(0);   // PB0
                SET_GPIO_B(7);   // PB7
                SET_GPIO_B(14);  // PB14
                SET_GPIO_B(15);  // PB15 (外接LED)
            } 
            else {
                // 超过六次按下，重置计数
                buttonPressCount = 0;
            }
        }
    }
}