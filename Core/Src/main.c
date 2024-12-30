#include "init.h"

int main(void) 
{
    GPIO_Ini();  // 初始化 GPIO

    uint8_t ledIndex = 0; // 当前点亮的 LED 索引
    uint8_t buttonPressCount = 0; // 按钮按下计数

    while(1)
    {
        // 检测按钮是否按下
        if (READ_GPIO_C13 == 0) {
            // 简单去抖动
            for (volatile int i = 0; i < 100000; i++);  // 延时
            while (READ_GPIO_C13 == 0);  // 等待按钮释放

            buttonPressCount++;  // 增加按钮按下计数

            // 按钮按下次数控制 LED
            if (buttonPressCount <= 4) {
                // 点亮下一个 LED
                if (ledIndex < 4) {
                    // 根据 ledIndex 点亮对应的 LED
                    switch (ledIndex) {
                        case 0:
                            SET_GPIO_B(0);  // PB0
                            break;
                        case 1:
                            SET_GPIO_B(7);  // PB7
                            break;
                        case 2:
                            SET_GPIO_B(14); // PB14
                            break;
                        case 3:
                            SET_GPIO_B(15); // PB15
                            break;
                        default:
                            break;
                    }
                    ledIndex++;  // 更新当前 LED 索引
                }
            } 
            else if (buttonPressCount == 5) {
                // 熄灭所有 LED
                RESET_GPIO_B(0);   // PB0
                RESET_GPIO_B(7);   // PB7
                RESET_GPIO_B(14);  // PB14
                RESET_GPIO_B(15);  // PB15 (外接 LED)
                ledIndex = 0; // 重置 LED 索引
            } 
            else if (buttonPressCount == 6) {
                // 点亮所有 LED
                SET_GPIO_B(0);   // PB0
                SET_GPIO_B(7);   // PB7
                SET_GPIO_B(14);  // PB14
                SET_GPIO_B(15);  // PB15 (外接 LED)
            } 
            else {
                // 超过六次按下，重置计数
                buttonPressCount = 0;
                RESET_GPIO_B(0);   // PB0
                RESET_GPIO_B(7);   // PB7
                RESET_GPIO_B(14);  // PB14
                RESET_GPIO_B(15);  // PB15 (外接 LED)
                ledIndex = 0;  // 重置 LED 索引
            }
        }
    }
}