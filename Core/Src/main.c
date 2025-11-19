#include "init.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"

// 全局变量 - 用于STM32Viewer观察
volatile uint8_t led_count = 1;         // 点亮的LED数量 (1->2->3->1 循环)
volatile uint8_t current_led = 0;       // 当前点亮的LED索引
volatile uint8_t led_initialized = 0;   // LED是否已初始化显示的标志
volatile uint8_t button1_state = 0;     // PC13按键状态（用于观察）
volatile uint8_t button2_state = 0;     // PC6按键状态（用于观察）
volatile uint8_t led_mode = 0;          // 当前LED模式：0=单灯,1=双灯,2=三灯
volatile uint32_t button1_press_count = 0;  // PC13按键按下次数统计
volatile uint32_t button2_press_count = 0;  // PC6按键按下次数统计
volatile uint8_t led_brightness[3] = {0, 0, 0};  // 三个LED的亮度状态：0=灭,1=亮
volatile uint32_t system_ticks = 0;     // 系统运行计数器

// LED状态数组 - 按照 PB14, PB7, PB0 的顺序
const uint32_t led_states[] = {LED1_ON, LED2_ON, LED3_ON};
const uint32_t led_off_states[] = {LED1_OFF, LED2_OFF, LED3_OFF};

void delay(volatile uint32_t s) {
    while (s--) {
        system_ticks++;  // 在延时中计数，用于观察系统运行状态
    }
}

void update_leds(uint8_t led_index, uint8_t count);
void update_led_brightness_array(uint8_t led_index, uint8_t count);

int main() {
    // 初始化GPIO
    GPIO_Ini_Self_Def();   // 调用自定义的GPIO初始化函数
    GPIO_Ini_CMSIS();      // 调用基于CMSIS库的GPIO初始化函数

    uint8_t button1_pressed = 0;  // PC13按键临时状态
    uint8_t button2_pressed = 0;  // PC6按键临时状态

    // 初始状态：关闭所有LED
    GPIOB->BSRR = LED1_OFF | LED2_OFF | LED3_OFF;
    // 初始化LED亮度数组
    update_led_brightness_array(current_led, led_count);

    while (1) {
        // 更新按键状态用于观察
        button1_state = (GPIOC->IDR & GPIO_IDR_ID13) ? 1 : 0;
        button2_state = (GPIOC->IDR & GPIO_IDR_ID6) ? 0 : 1;  // 内部上拉，按下为低电平

        // ========== 按钮1逻辑（PC13 - 外部下拉，按下时为高电平）==========
        if (GPIOC->IDR & GPIO_IDR_ID13) {
            if (!button1_pressed) {
                button1_pressed = 1;  // 标记按键为按下状态
                button1_press_count++;  // 增加按键按下计数
                
                // 设置LED已初始化标志
                led_initialized = 1;
                
                // 根据当前LED数量模式进行不同的切换操作
                if (led_count == 1) {
                    // 单灯模式：依次点亮一个LED
                    current_led = (current_led + 1) % 3;
                    update_leds(current_led, 1);
                    led_mode = 0;  // 单灯模式
                } else if (led_count == 2) {
                    // 双灯模式：依次点亮两个LED
                    current_led = (current_led + 1) % 3;
                    update_leds(current_led, 2);
                    led_mode = 1;  // 双灯模式
                } else if (led_count == 3) {
                    // 三灯模式：切换所有LED的开关状态
                    static uint8_t all_on = 0;
                    all_on = !all_on;
                    if (all_on) {
                        GPIOB->BSRR = LED1_ON | LED2_ON | LED3_ON;
                        // 更新亮度数组 - 全部点亮
                        for(int i = 0; i < 3; i++) {
                            led_brightness[i] = 1;
                        }
                    } else {
                        GPIOB->BSRR = LED1_OFF | LED2_OFF | LED3_OFF;
                        // 更新亮度数组 - 全部熄灭
                        for(int i = 0; i < 3; i++) {
                            led_brightness[i] = 0;
                        }
                    }
                    led_mode = 2;  // 三灯模式
                }
                
                // 更新LED亮度状态数组
                update_led_brightness_array(current_led, led_count);
            }
        } else {
            button1_pressed = 0;
        }

        // ========== 按钮2逻辑（PC6 - 内部上拉，按下时为低电平）==========
        if (!(GPIOC->IDR & GPIO_IDR_ID6)) {
            if (!button2_pressed) {
                button2_pressed = 1;
                button2_press_count++;  // 增加按键按下计数
                
                // 切换LED数量模式 (1->2->3->1 循环)
                led_count = (led_count % 3) + 1;
                current_led = 0;  // 重置到第一个LED
                led_initialized = 0;  // 重置初始化标志
                
                // 更新模式标识
                if (led_count == 1) led_mode = 0;
                else if (led_count == 2) led_mode = 1;
                else if (led_count == 3) led_mode = 2;
                
                // 根据新模式更新LED显示
                if (led_count == 3) {
                    // 三灯模式：初始状态为全灭
                    GPIOB->BSRR = LED1_OFF | LED2_OFF | LED3_OFF;
                    // 更新亮度数组
                    for(int i = 0; i < 3; i++) {
                        led_brightness[i] = 0;
                    }
                } else {
                    // 单灯或双灯模式：初始状态为全灭，等待PC13按键按下
                    GPIOB->BSRR = LED1_OFF | LED2_OFF | LED3_OFF;
                    // 更新亮度数组
                    for(int i = 0; i < 3; i++) {
                        led_brightness[i] = 0;
                    }
                }
            }
        } else {
            button2_pressed = 0;
        }

        // ========== 双灯模式特殊处理：如果未初始化且是双灯模式，保持熄灭 ==========
        if (led_count == 2 && !led_initialized) {
            // 保持所有LED熄灭状态，等待PC13按键按下
            GPIOB->BSRR = LED1_OFF | LED2_OFF | LED3_OFF;
            // 更新亮度数组
            for(int i = 0; i < 3; i++) {
                led_brightness[i] = 0;
            }
        }

        system_ticks++;  // 增加系统运行计数
        delay(100000);   // 去抖延时
    }
}

// 更新LED状态的函数
void update_leds(uint8_t led_index, uint8_t count) {
    // 关闭所有LED
    GPIOB->BSRR = LED1_OFF | LED2_OFF | LED3_OFF;

    // 如果LED已经初始化（即PC13按键已按下过），则点亮LED
    if (led_initialized) {
        // 点亮指定数量的LED（按照 PB14, PB7, PB0 的顺序循环）
        for (uint8_t i = 0; i < count; i++) {
            uint8_t index = (led_index + i) % 3;
            GPIOB->BSRR = led_states[index];
        }
    }
}

// 更新LED亮度状态数组的函数
void update_led_brightness_array(uint8_t led_index, uint8_t count) {
    // 先重置所有LED状态为0（熄灭）
    for(int i = 0; i < 3; i++) {
        led_brightness[i] = 0;
    }
    
    // 如果LED已经初始化，则设置对应的亮度状态
    if (led_initialized) {
        for (uint8_t i = 0; i < count; i++) {
            uint8_t index = (led_index + i) % 3;
            led_brightness[index] = 1;  // 设置为1（点亮）
        }
    }
}
