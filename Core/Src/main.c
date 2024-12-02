#include <stdint.h> 

int main(void) {
    // 1. 启用 GPIOB 和 GPIOC 的时钟
    *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x06; // 启用 GPIOB 和 GPIOC 的时钟

    // 2. 配置 GPIOB 的 LED 引脚为输出模式
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x01;    // 将 PB0 配置为输出模式
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x4000;  // 将 PB7 配置为输出模式
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x100000; // 将 PB14 配置为输出模式

    // 3. 配置 GPIOB 的输出模式为推挽模式
    *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x00; // 配置 PB0、PB7 和 PB14 的输出模式为推挽模式

    // 4. 配置 GPIOB 的输出速度为中速
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x01;    // 将 PB0 配置为中速模式
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x4000;  // 将 PB7 配置为中速模式
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x100000; // 将 PB14 配置为中速模式

    // 5. 禁用 GPIOB 的上拉/下拉电阻
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x00; // 禁用 PB0、PB7 和 PB14 的上拉/下拉电阻

    // 6. 配置 GPIOC 的按钮引脚为输入模式
    *(uint32_t *)(0x40020800UL + 0x00UL) |= 0x00; // 将 PC13 配置为输入模式
    *(uint32_t *)(0x40020800UL + 0x00UL) |= 0x00; // 将 PC6 配置为输入模式

    // 7. 禁用 GPIOC 的上拉/下拉电阻
    *(uint32_t *)(0x40020800UL + 0x0CUL) |= 0x00; // 禁用 PC13 和 PC6 的上拉/下拉电阻

    // 状态变量
    uint8_t led_state = 0; // 当前点亮的 LED（0=PB0, 1=PB7, 2=PB14）
    uint8_t mode = 1;      // 当前模式：1 = 点亮 1 个 LED，2 = 点亮 2 个 LED

    // 主循环
    while (1) {
        // 检测按钮 PC13 (BTN1)
        if ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x2000) != 0) { // 检查 PC13 引脚是否为高电平
            for (volatile int i = 0; i < 50000; i++); // 简单去抖延迟
            if ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x2000) != 0) {
                led_state = (led_state + 1) % 3; // 切换到下一个 LED
                *(uint32_t *)(0x40020400UL + 0x18UL) &= ~0x4081; // 关闭所有 LED
                if (mode == 1) {
                    *(uint32_t *)(0x40020400UL + 0x18UL) |= (led_state == 0 ? 0x01 : (led_state == 1 ? 0x80 : 0x4000));
                } else if (mode == 2) {
                    if (led_state == 0) *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x81; // 点亮 PB0 和 PB7
                    else if (led_state == 1) *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x4080; // 点亮 PB7 和 PB14
                    else *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x4001; // 点亮 PB14 和 PB0
                }
                while ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x2000) != 0); // 等待按钮释放
            }
        }

        // 检测按钮 PC6 (BTN2)
        if ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x40) != 0) { // 检查 PC6 引脚是否为高电平
            for (volatile int i = 0; i < 50000; i++); // 简单去抖延迟
            if ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x40) != 0) {
                mode = (mode == 1) ? 2 : 1; // 切换模式
                *(uint32_t *)(0x40020400UL + 0x18UL) &= ~0x4081; // 关闭所有 LED
                if (mode == 1) {
                    *(uint32_t *)(0x40020400UL + 0x18UL) |= (led_state == 0 ? 0x01 : (led_state == 1 ? 0x80 : 0x4000));
                } else if (mode == 2) {
                    if (led_state == 0) *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x81; // 点亮 PB0 和 PB7
                    else if (led_state == 1) *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x4080; // 点亮 PB7 和 PB14
                    else *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x4001; // 点亮 PB14 和 PB0
                }
                while ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x40) != 0); // 等待按钮释放
            }
        }
    }
}