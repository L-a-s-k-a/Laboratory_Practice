#include "init.h"

// 简单延时函数（毫秒级）
static void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 1000U; i++) { __NOP(); }
}

// 读取GPIO引脚电平
static inline uint32_t read_bit_idr(uint32_t gpio_base, uint8_t pin) {
    volatile uint32_t *idr = (volatile uint32_t *)(gpio_base + IDR_OFFSET);
    return (*idr) & (1U << pin);
}

// 检测PC12按钮状态（内部上拉）- 按下返回1，释放返回0
static uint8_t get_pc12_button_state(void) {
    uint32_t raw = read_bit_idr(GPIOC_BASE_ADDR, 12);
    // 内部上拉：低电平=按下，高电平=释放
    button1_flag = (raw == 0U) ? 1U : 0U;  // 更新按钮1标志
    return button1_flag;
}

// 检测PC6按钮状态 - 改进的消抖检测
static uint8_t check_pc6_button_press(void) {
    static uint8_t last_state = 1;  // 上次状态，初始为释放（上拉时为高电平）
    uint8_t current_state = (read_bit_idr(GPIOC_BASE_ADDR, 6) == 0U) ? 1U : 0U;
    
    // 检测按下事件（从释放到按下）
    if (last_state == 0 && current_state == 1) {
        delay_ms(20);  // 消抖延时
        // 确认仍然是按下状态
        if ((read_bit_idr(GPIOC_BASE_ADDR, 6) == 0U)) {
            last_state = current_state;
            button2_flag = 1;  // 更新按钮2标志
            return 1;  // 返回有效按下
        }
    }
    
    last_state = current_state;
    button2_flag = 0;  // 更新按钮2标志
    return 0;
}

// 检测PC8按钮状态（内部上拉）- 按下返回1，释放返回0
static uint8_t get_pc8_button_state(void) {
    uint32_t raw = read_bit_idr(GPIOC_BASE_ADDR, 8);
    // 内部上拉：低电平=按下，高电平=释放
    button3_flag = (raw == 0U) ? 1U : 0U;  // 更新按钮3标志
    return button3_flag;
}

// 切换PC8模式并点亮蓝色LED
static void toggle_pc8_mode_with_led(void) {
    // 点亮蓝色LED（PB7）
    PB7_LED_ON();
    led_2_state = 1;  // 更新蓝色LED状态
    
    // 切换PC8模式
    if (g_pc8_mode == 0) {
        // 当前是输入模式，切换到输出高电平模式
        PC8_Set_Output_High();
    } else {
        // 当前是输出模式，切换回输入模式
        PC8_Set_Input_PullUp();
    }
    
    // 蓝色LED保持点亮一段时间，提供视觉反馈
    delay_ms(300);  // 点亮300毫秒
    
    // 熄灭蓝色LED
    PB7_LED_OFF();
    led_2_state = 0;  // 更新蓝色LED状态
}

// 主函数
int main(void) {
    // 初始化GPIO
    GPIO_Init_Memory();

    // 初始状态设置 - 所有LED熄灭
    led_off(GPIOB, 14);  // 关闭PB14 LED
    PB7_LED_OFF();       // 关闭PB7 LED（蓝色）
    pb0_led_off();       // 关闭PB0 LED（黄色）
    PB8_LED_OFF();       // 关闭PB8 LED

    // 更新所有LED状态为熄灭
    led_1_state = 0;  // 红色LED熄灭
    led_2_state = 0;  // 蓝色LED熄灭  
    led_3_state = 0;  // 黄色LED熄灭
    led_4_state = 0;  // 外部LED熄灭

    // PC8初始状态为按键输入模式（已在初始化中设置）

    // 主循环
    while (1) {
        // 检测PC12按钮状态并控制PB14 LED
        if (get_pc12_button_state()) {
            led_on(GPIOB, 14);     // 按下时点亮PB14 LED
            led_1_state = 1;       // 更新红色LED状态
        } else {
            led_off(GPIOB, 14);    // 释放时熄灭PB14 LED
            led_1_state = 0;       // 更新红色LED状态
        }

        // 检测PC6按钮：切换PC8模式并点亮蓝色LED
        if (check_pc6_button_press()) {
            toggle_pc8_mode_with_led();  // 切换PC8模式并点亮蓝色LED
        }

        // 根据PC8模式处理PC8按钮
        if (g_pc8_mode == 0) {
            // PC8为输入模式：检测PC8按钮控制黄色LED
            if (get_pc8_button_state()) {
                pb0_led_on();      // 按下时点亮PB0黄色LED
                led_3_state = 1;   // 更新黄色LED状态
            } else {
                pb0_led_off();     // 释放时熄灭PB0黄色LED
                led_3_state = 0;   // 更新黄色LED状态
            }
        } else {
            // PC8为输出模式：PC8持续输出高电平，外接LED点亮
            // 黄色LED保持熄灭
            pb0_led_off();         // 确保黄色LED熄灭
            led_3_state = 0;       // 更新黄色LED状态
        }

        // 短延时，降低CPU占用率
        delay_ms(10);
    }
}
