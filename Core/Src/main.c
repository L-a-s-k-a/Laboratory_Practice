#include "E:/VSCode-file/Laboratory_Practice/CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "E:/VSCode-file/Laboratory_Practice/Core/Inc/init.h" 

// 本地按键状态变量（用于消抖逻辑）
static uint8_t last_pg3_raw_state = 1;
static uint8_t last_pc6_raw_state = 1;
static uint8_t last_pc8_raw_state = 1;
static uint8_t last_pc9_raw_state = 1;

// 简单延时函数
static void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 1000U; i++) {
        __NOP();
    }
}

// 模式切换按键检测 - 纯地址级直接内存访问
static uint8_t check_mode_button(void) {
    // 读取PG3原始状态（内部上拉：1=未按下，0=按下）
    uint8_t current_raw_state = (*((volatile uint32_t*)(GPIOG_BASE + IDR_OFFSET)) & (1U << KEY_MODE_PIN)) ? 1 : 0;
    
    // 更新监测变量 - 直接反映引脚电平
    g_system_monitor.pg3_key_state = current_raw_state;
    
    // 检测下降沿（从1到0）
    if (current_raw_state == 0 && last_pg3_raw_state == 1) {
        delay_ms(20);  // 消抖
        current_raw_state = (*((volatile uint32_t*)(GPIOG_BASE + IDR_OFFSET)) & (1U << KEY_MODE_PIN)) ? 1 : 0;
        g_system_monitor.pg3_key_state = current_raw_state;  // 更新消抖后的状态
        
        if (current_raw_state == 0) {
            // 等待按键释放
            while ((*((volatile uint32_t*)(GPIOG_BASE + IDR_OFFSET)) & (1U << KEY_MODE_PIN)) == 0) {
                g_system_monitor.pg3_key_state = 0;  // 持续更新为按下状态
            }
            delay_ms(20);
            
            // 按键释放后更新状态
            g_system_monitor.pg3_key_state = 1;  // 释放后回到未按下状态
            last_pg3_raw_state = 1;
            return 1;  // 返回有效按下
        }
    }
    
    last_pg3_raw_state = current_raw_state;
    return 0;
}

// 控制按键检测（用于按住点亮功能）- 修复volatile限定符
static uint8_t check_control_button_pressed(uint32_t gpio_base, uint16_t pin, uint8_t *last_raw_state, 
                                           volatile uint8_t *monitor_key_state, volatile uint8_t *pressed_flag) {
    // 读取原始引脚状态
    uint8_t current_raw_state = (*((volatile uint32_t*)(gpio_base + IDR_OFFSET)) & (1U << pin)) ? 1 : 0;
    
    // 更新监测变量 - 直接反映引脚电平
    *monitor_key_state = current_raw_state;
    *pressed_flag = 0;  // 清除按下标志
    
    // 检测按键是否被按住
    if (current_raw_state == 0) {
        delay_ms(20);  // 消抖
        current_raw_state = (*((volatile uint32_t*)(gpio_base + IDR_OFFSET)) & (1U << pin)) ? 1 : 0;
        *monitor_key_state = current_raw_state;
        
        if (current_raw_state == 0) {
            *pressed_flag = 1;  // 设置按下标志
            *last_raw_state = current_raw_state;
            return 1;  // 按键被按住
        }
    }
    
    *last_raw_state = current_raw_state;
    return 0;  // 按键未按住
}

// 模式控制函数
static void process_mode1_controls(void) {
    uint8_t pc6_pressed = check_control_button_pressed(GPIOC_BASE, KEY_CTRL1_PIN, &last_pc6_raw_state, &g_system_monitor.pc6_key_state, &g_system_monitor.pc6_pressed_flag);
    uint8_t pc8_pressed = check_control_button_pressed(GPIOC_BASE, KEY_CTRL2_PIN, &last_pc8_raw_state, &g_system_monitor.pc8_key_state, &g_system_monitor.pc8_pressed_flag);
    uint8_t pc9_pressed = check_control_button_pressed(GPIOC_BASE, KEY_CTRL3_PIN, &last_pc9_raw_state, &g_system_monitor.pc9_key_state, &g_system_monitor.pc9_pressed_flag);
    
    // PC6控制绿色LED组 - 按住点亮，松开熄灭
    if (pc6_pressed) {
        Green_LEDs_On();
    } else if (g_system_monitor.pc6_key_state == 1 && (g_system_monitor.onboard_green_state || g_system_monitor.external_green_state)) {
        Green_LEDs_Off();
    }
    
    // PC8控制蓝色LED组 - 按住点亮，松开熄灭
    if (pc8_pressed) {
        Blue_LEDs_On();
    } else if (g_system_monitor.pc8_key_state == 1 && (g_system_monitor.onboard_blue_state || g_system_monitor.external_blue_state)) {
        Blue_LEDs_Off();
    }
    
    // PC9控制红色LED组 - 按住点亮，松开熄灭
    if (pc9_pressed) {
        Red_LEDs_On();
    } else if (g_system_monitor.pc9_key_state == 1 && (g_system_monitor.onboard_red_state || g_system_monitor.external_red_state)) {
        Red_LEDs_Off();
    }
}

static void process_mode2_controls(void) {
    uint8_t pc6_pressed = check_control_button_pressed(GPIOC_BASE, KEY_CTRL1_PIN, &last_pc6_raw_state, &g_system_monitor.pc6_key_state, &g_system_monitor.pc6_pressed_flag);
    uint8_t pc8_pressed = check_control_button_pressed(GPIOC_BASE, KEY_CTRL2_PIN, &last_pc8_raw_state, &g_system_monitor.pc8_key_state, &g_system_monitor.pc8_pressed_flag);
    uint8_t pc9_pressed = check_control_button_pressed(GPIOC_BASE, KEY_CTRL3_PIN, &last_pc9_raw_state, &g_system_monitor.pc9_key_state, &g_system_monitor.pc9_pressed_flag);
    
    // PC6控制红色LED组
    if (pc6_pressed) {
        Red_LEDs_On();
    } else if (g_system_monitor.pc6_key_state == 1 && (g_system_monitor.onboard_red_state || g_system_monitor.external_red_state)) {
        Red_LEDs_Off();
    }
    
    // PC8控制绿色LED组
    if (pc8_pressed) {
        Green_LEDs_On();
    } else if (g_system_monitor.pc8_key_state == 1 && (g_system_monitor.onboard_green_state || g_system_monitor.external_green_state)) {
        Green_LEDs_Off();
    }
    
    // PC9控制蓝色LED组
    if (pc9_pressed) {
        Blue_LEDs_On();
    } else if (g_system_monitor.pc9_key_state == 1 && (g_system_monitor.onboard_blue_state || g_system_monitor.external_blue_state)) {
        Blue_LEDs_Off();
    }
}

static void process_mode3_controls(void) {
    uint8_t pc6_pressed = check_control_button_pressed(GPIOC_BASE, KEY_CTRL1_PIN, &last_pc6_raw_state, &g_system_monitor.pc6_key_state, &g_system_monitor.pc6_pressed_flag);
    uint8_t pc8_pressed = check_control_button_pressed(GPIOC_BASE, KEY_CTRL2_PIN, &last_pc8_raw_state, &g_system_monitor.pc8_key_state, &g_system_monitor.pc8_pressed_flag);
    uint8_t pc9_pressed = check_control_button_pressed(GPIOC_BASE, KEY_CTRL3_PIN, &last_pc9_raw_state, &g_system_monitor.pc9_key_state, &g_system_monitor.pc9_pressed_flag);
    
    // PC6控制蓝色LED组
    if (pc6_pressed) {
        Blue_LEDs_On();
    } else if (g_system_monitor.pc6_key_state == 1 && (g_system_monitor.onboard_blue_state || g_system_monitor.external_blue_state)) {
        Blue_LEDs_Off();
    }
    
    // PC8控制红色LED组
    if (pc8_pressed) {
        Red_LEDs_On();
    } else if (g_system_monitor.pc8_key_state == 1 && (g_system_monitor.onboard_red_state || g_system_monitor.external_red_state)) {
        Red_LEDs_Off();
    }
    
    // PC9控制绿色LED组
    if (pc9_pressed) {
        Green_LEDs_On();
    } else if (g_system_monitor.pc9_key_state == 1 && (g_system_monitor.onboard_green_state || g_system_monitor.external_green_state)) {
        Green_LEDs_Off();
    }
}

static void process_control_buttons(void) {
    switch (g_system_monitor.current_mode) {
        case 1: 
            process_mode1_controls(); 
            break;
        case 2: 
            process_mode2_controls(); 
            break;
        case 3: 
            process_mode3_controls(); 
            break;
        default: 
            g_system_monitor.current_mode = 1; 
            break;
    }
}

// 新增：PG3脉冲标志管理函数
static void manage_pg3_pulse_flag(void) {
    if (g_system_monitor.pg3_pulse_counter > 0) {
        g_system_monitor.pg3_pulse_counter--;
        
        // 当计数器减到0时，清除脉冲标志
        if (g_system_monitor.pg3_pulse_counter == 0) {
            g_system_monitor.pg3_pressed_flag = 0;  // 产生脉冲下降沿
        }
    }
}

// 主函数
int main(void) {
    GPIO_Init_Memory();
    All_LEDs_Off();
    g_system_monitor.current_mode = 1;

    while (1) {
        // 更新循环计数器
        g_system_monitor.loop_counter++;
        
        // 检测PG3模式切换按键
        if (check_mode_button()) {
            Switch_Mode();
        }

        // 处理控制按键
        process_control_buttons();
        
        // 新增：管理PG3脉冲标志
        manage_pg3_pulse_flag();
        
        // 短延时，降低CPU占用率
        delay_ms(10);
    }
}