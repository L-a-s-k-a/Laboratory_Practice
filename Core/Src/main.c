#include "init.h"

//全局监控变量
volatile uint8_t  g_pd4_button_state  = 0;     // PD4按钮状态
volatile uint32_t g_pd4_raw_value     = 0;     // PD4原始电平值
volatile uint8_t  g_pd4_pressed_flag  = 0;     // PD4按下标志位
volatile uint32_t g_button_debug_counter = 0;  // 按钮调试计数器
volatile uint8_t  g_led_step   = 0;            // LED步进值: 0=全灭, 1=PB14, 2=PB7, 3=PB0
volatile uint8_t  g_pg3_button_state  = 0;     // PG3按钮状态
volatile uint8_t  g_pg3_pressed_flag  = 0;     // PG3按下标志位
volatile uint8_t  g_system_mode = 0;           // 系统模式: 0=正常模式, 1=PD4输出模式
volatile uint8_t  g_pd4_mode_state = 0;        // PD4引脚模式状态: 0=输入上拉, 1=输出高电平, 2=输入无上下拉

//简单延时函数（毫秒级）

static void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 1000U; i++) { __NOP(); }
}


//LED控制宏函数 - 点亮指定GPIO的LED

static inline void led_on(GPIO_TypeDef* port, uint8_t pin)  { port->BSRR = (1U << pin); }


//LED控制宏函数 - 熄灭指定GPIO的LED

static inline void led_off(GPIO_TypeDef* port, uint8_t pin) { port->BSRR = (1U << (pin + 16U)); }


//专门控制PB0 LED的点亮函数 - 直接内存访问

static inline void pb0_led_on(void)  { *(volatile uint32_t *)(GPIOB_BASE_ADDR + BSRR_OFFSET) = (1U << 0); }


//专门控制PB0 LED的熄灭函数 - 直接内存访问

static inline void pb0_led_off(void) { *(volatile uint32_t *)(GPIOB_BASE_ADDR + BSRR_OFFSET) = (1U << (0 + 16U)); }


//读取GPIO引脚电平
static inline uint32_t read_bit_idr(uint32_t gpio_base, uint8_t pin) {
    volatile uint32_t *idr = (volatile uint32_t *)(gpio_base + IDR_OFFSET);
    return (*idr) & (1U << pin);
}


//更新PD4按钮状态到全局变量

static void update_pd4_button_status(uint32_t raw_value, uint8_t pressed) {
    g_pd4_raw_value = raw_value;
    // 内部上拉电阻: 低电平=按下，高电平=释放
    g_pd4_button_state = (raw_value == 0U) ? 1U : 0U;//逻辑反转
    if (pressed) { 
        g_pd4_pressed_flag = 1U; 
        g_button_debug_counter++; 
    }
}


//更新PG3按钮状态到全局变量

static void update_pg3_button_status(uint32_t raw_value, uint8_t pressed) {
    // 内部上拉电阻: 低电平=按下，高电平=释放
    g_pg3_button_state = (raw_value == 0U) ? 1U : 0U;
    if (pressed) { 
        g_pg3_pressed_flag = 1U; 
        g_button_debug_counter++; 
    }
}

//按键检测函数（带消抖处理）


//检测PD4按钮（带消抖，按下并释放后返回1）

static uint8_t check_pd4_button(void) {
    // 如果系统处于PD4输出模式，PD4按键不工作
    if (g_system_mode == 1) {
        return 0;
    }
    
    uint8_t result = 0;
    
    uint32_t raw = read_bit_idr(GPIOD_BASE_ADDR, 4);
    update_pd4_button_status(raw, 0);

    // 检测到低电平（可能按下）
    if (raw == 0U) {
        delay_ms(20);  // 延时消抖
        raw = read_bit_idr(GPIOD_BASE_ADDR, 4);
        update_pd4_button_status(raw, 0);
        
        // 再次确认低电平（确认按下）
        if (raw == 0U) {
            // 等待按钮释放
            while (read_bit_idr(GPIOD_BASE_ADDR, 4) == 0U) {
                update_pd4_button_status(read_bit_idr(GPIOD_BASE_ADDR, 4), 0);
            }
            result = 1;  // 标记有效按下
            update_pd4_button_status(read_bit_idr(GPIOD_BASE_ADDR, 4), 1);
        }
    }
    
    return result;
}


//检测PG3按钮（带消抖，按下并释放后返回1）

static uint8_t check_pg3_button(void) {
    uint8_t result = 0;
    uint32_t raw = read_bit_idr(GPIOG_BASE_ADDR, 3);
    update_pg3_button_status(raw, 0);

    // 检测到低电平（可能按下）
    if (raw == 0U) {
        delay_ms(20);  // 延时消抖
        raw = read_bit_idr(GPIOG_BASE_ADDR, 3);
        update_pg3_button_status(raw, 0);
        
        // 再次确认低电平（确认按下）
        if (raw == 0U) {
            // 等待按钮释放
            while (read_bit_idr(GPIOG_BASE_ADDR, 3) == 0U) {
                update_pg3_button_status(read_bit_idr(GPIOG_BASE_ADDR, 3), 0);
            }
            result = 1;  // 标记有效按下
            update_pg3_button_status(read_bit_idr(GPIOG_BASE_ADDR, 3), 1);
        }
    }
    return result;
}

//LED控制函数

//关闭所有工作LED（PB14, PB7, PB0）

static void all_work_leds_off(void) {
    led_off(GPIOB, 14);  // 关闭PB14 LED
    PB7_LED_OFF();       // 关闭PB7 LED（宏函数）
    pb0_led_off();       // 关闭PB0 LED（直接内存访问）
}

/*
点亮单个LED（熄灭其他所有LED）
步进值: 0=全灭, 1=PB14, 2=PB7, 3=PB0
 */
static void turn_on_single_led(uint8_t step) {
    // 先关闭所有LED
    all_work_leds_off();
    
    // 根据步进值点亮对应的单个LED
    switch (step) {
        case 1: 
            led_on(GPIOB, 14);  // 只点亮PB14
            break; 
        case 2: 
            led_on(GPIOB, 14);  // 只点亮PB14
            PB7_LED_ON();       // 只点亮PB7（宏控制）
            break; 
        case 3:
            led_on(GPIOB, 14);  // 只点亮PB14
            PB7_LED_ON();       // 只点亮PB7（宏控制） 
            pb0_led_on();       // 只点亮PB0（直接内存控制）
            break; 
        case 0: 
        default: 
            // 全灭状态，不点亮任何LED
            break;
    }
}


//切换系统模式

static void toggle_system_mode(void) {
    if (g_system_mode == 0) {
        // 切换到PD4输出模式
        g_system_mode = 1;
        
        // 设置PD4为输出高电平
        PD4_Set_Output_High();
        
        // 关闭所有工作LED
        all_work_leds_off();
        
    } else {
        // 切换回正常模式
        g_system_mode = 0;
        
        // 设置PD4为输入上拉（恢复正常按键功能，避免浮空）
        PD4_Set_Input_PullUp();
        
        // 重置LED状态为全灭
        g_led_step = 0;
        turn_on_single_led(g_led_step);  // 所有LED熄灭
    }
}

//主函数
int main(void) {
    // 初始化GPIO和相关硬件
    GPIO_Init_Memory();

    // 初始状态设置
    all_work_leds_off();    // 所有工作LED熄灭
    g_led_step = 0;         // 初始步进值：全灭
    g_system_mode = 0;      // 初始为正常模式
    g_pd4_mode_state = 0;   // 初始PD4模式：输入上拉

    // 主循环
    while (1) {
        // 清除按下标志位
        g_pd4_pressed_flag = 0;
        g_pg3_pressed_flag = 0;

        // 检测PG3按钮：系统模式切换
        if (check_pg3_button()) {
            toggle_system_mode();  // 切换系统模式
        }

        // 检测PD4按钮：LED顺序点亮控制（仅在正常模式下工作）
        if (check_pd4_button()) {
            // 步进值循环：0→1→2→3→0
            g_led_step = (uint8_t)((g_led_step + 1U) % 4U);
            
            // 点亮对应的单个LED
            turn_on_single_led(g_led_step);
        }

        // 短延时，降低CPU占用率///
        delay_ms(10);
    }
}