#include <stdint.h>

int main(void) {
    // 开启 GPIOB 和 GPIOC 的时钟
    *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x06; // 使能 GPIOB 和 GPIOC 时钟

    // 配置 GPIOB 7 引脚为输出模式
    *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; // 设置为输出模式
    *(uint32_t*)(0x40020400UL + 0x04UL) &= ~0x4000; // 设置为推挽输出
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; // 设置速度为中等
    *(uint32_t*)(0x40020400UL + 0x0CUL) &= ~0x4000; // 禁用 PU/PD

    // 配置 GPIOB 8 和 9 引脚为输入模式（按钮）
    *(uint32_t*)(0x40020400UL + 0x00UL) &= ~0x0300; // 设置为输入模式
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x0300; // 启用 PU/PD（上拉）

    while (1) {
        // 检查按钮 1 (GPIOB 8)
        if ((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x0100UL) == 0) {
            // 如果按钮 1 被按下，点亮第一个 LED (GPIOB 7)
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80; // 点亮 LED
        } else {
            // 否则，熄灭 LED
            *(uint32_t*)(0x40020400UL + 0x18UL) &= ~0x80; // 熄灭 LED
        }

        // 检查按钮 2 (GPIOB 9)
        if ((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x0200UL) == 0) {
            // 如果按钮 2 被按下，点亮第二个 LED (GPIOB 7)
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80; // 点亮 LED
            // 可以在这里添加代码以控制其他 LED
        }
    }
}
