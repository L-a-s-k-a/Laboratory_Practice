#include "init.h"  // 包含自定义初始化头文件，包含所有宏定义和寄存器地址定义

static void delay_ms(uint32_t ms) {// 简单的软件延时函数
    for (uint32_t i = 0; i < ms * 1000; i++) {  // 循环ms*1000次，每次循环约1微秒
        __NOP();  // 不进行任何实际操作，只消耗CPU时间
    }
}
// 检测PC13按键状态函数
// 返回值：1-按键被按下并释放，0-按键未被按下
// PC13按键特性：按下时为高电平，未按下时为低电平（使用外部下拉电阻）
static uint8_t check_pc13_button(void) {
    // 获取PC13输入数据寄存器的内存地址（直接内存访问）
    volatile uint32_t *pc13_idr = (volatile uint32_t *)(GPIOC_BASE_ADDR + IDR_OFFSET);
    // 检测PC13引脚是否为高电平（按键按下）
    if ((*pc13_idr & (1 << 13)) != 0) {
        delay_ms(20);  // 延时20毫秒进行按键消抖，消除机械抖动
        if ((*pc13_idr & (1 << 13)) != 0) {
            // 等待按键释放（保持检测直到引脚变为低电平）
            while ((*pc13_idr & (1 << 13)) != 0);
            return 1;  // 返回1表示检测到有效的按键按下并释放
        }
    }
    return 0;  // 返回0表示没有检测到按键按下
}

// 检测PD4按键状态函数
// 返回值：1-按键被按下并释放，0-按键未被按下
// PD4按键特性：按下时为低电平，未按下时为高电平（使用内部上拉电阻）
// PD4同时连接LED和按键，需要临时切换模式
static uint8_t check_pd4_button(void) {
    // 获取PD4模式寄存器的内存地址（直接内存访问）
    volatile uint32_t *pd4_moder = (volatile uint32_t *)(GPIOD_BASE_ADDR + MODER_OFFSET);
    // 临时将PD4切换为输入模式来检测按键（因为PD4默认是输出模式控制LED）
    *pd4_moder &= ~(3 << (4 * 2));  // 清除PD4的模式位，设置为输入模式(00)
    delay_ms(1);  // 短暂延时确保GPIO模式切换完成
    
    uint8_t result = 0;  // 初始化检测结果为0（未按下）
    
    // 获取PD4输入数据寄存器的内存地址
    volatile uint32_t *pd4_idr = (volatile uint32_t *)(GPIOD_BASE_ADDR + IDR_OFFSET);
    
    // 检测PD4引脚是否为低电平（按键按下）
    if ((*pd4_idr & (1 << 4)) == 0) {
        delay_ms(20);  // 延时20毫秒进行按键消抖
        // 再次检测确认按键确实按下
        if ((*pd4_idr & (1 << 4)) == 0) {
            // 等待按键释放（保持检测直到引脚变为高电平）
            while ((*pd4_idr & (1 << 4)) == 0);
            result = 1;  // 设置结果为1表示检测到有效的按键按下
        }
    }
    
    // 恢复PD4为输出模式（用于控制LED）
    *pd4_moder = (*pd4_moder & ~(3 << (4 * 2))) | (1 << (4 * 2));  // 设置PD4为输出模式(01)
    return result;  // 返回按键检测结果
}

// LED控制函数 - 点亮指定端口的LED
// 参数：port - GPIO端口指针，pin - 引脚编号
// 原理：设置BSRR寄存器的置位位来点亮LED
static inline void led_on(GPIO_TypeDef* port, uint8_t pin) {
    port->BSRR = (1 << pin);  // 设置BSRR寄存器的对应位，将引脚输出高电平
}

// LED控制函数 - 熄灭指定端口的LED
// 参数：port - GPIO端口指针，pin - 引脚编号
// 原理：设置BSRR寄存器的高16位复位位来熄灭LED
static inline void led_off(GPIO_TypeDef* port, uint8_t pin) {
    port->BSRR = (1 << (pin + 16));  // 设置BSRR寄存器的高16位对应位，将引脚输出低电平
}

// PB0 LED控制函数 使用真正的直接内存访问点亮LED 使用硬编码地址直接操作寄存器
static inline void pb0_led_on(void) {
    // 获取PB0位设置/复位寄存器的内存地址（直接硬编码地址访问）
    volatile uint32_t *pb0_bsrr = (volatile uint32_t *)(GPIOB_BASE_ADDR + BSRR_OFFSET);
    *pb0_bsrr = (1 << 0);  // 设置BS0位，点亮PB0连接的LED
}
// PB0 LED控制函数 - 使用真正的直接内存访问熄灭LED
static inline void pb0_led_off(void) {
    // 获取PB0位设置/复位寄存器的内存地址（直接硬编码地址访问）
    volatile uint32_t *pb0_bsrr = (volatile uint32_t *)(GPIOB_BASE_ADDR + BSRR_OFFSET);
    *pb0_bsrr = (1 << (0 + 16));  // 设置BR0位，熄灭PB0连接的LED
}
// 主函数
int main(void) {
    // 初始化所有GPIO引脚（配置输入输出模式、速度、上下拉等）
    GPIO_Init_Memory();
    
    // 上电时关闭所有LED，确保初始状态为熄灭
    led_off(GPIOD, 4);    // 熄灭PD4连接的LED（LED1）
    led_off(GPIOB, 14);   // 熄灭PB14连接的LED（LED2）
    PB7_LED_OFF();        // 使用宏熄灭PB7连接的LED（LED3） 使用自定义宏
    pb0_led_off();        // 使用直接内存访问熄灭PB0连接的LED（LED4） 使用直接内存访问
    
    // 初始化状态变量
    uint8_t mode = 0;         // 工作模式：0=正常模式，1=PD4按键模式
    uint8_t pb14_enabled = 1; // PB14 LED使能状态：1=参与跑马灯，0=不参与
    uint8_t pb7_enabled = 1;  // PB7 LED使能状态：1=参与跑马灯，0=不参与
    uint8_t pd4_enabled = 1;  // PD4 LED使能状态：1=参与跑马灯，0=不参与

    // 主循环 - 程序持续运行
    while (1) {
        // 检测PC13按键（用户输入）
        if (check_pc13_button()) {
            if (mode == 0) {
                // 正常模式下：PC13按键切换PB14 LED的使能状态
                pb14_enabled = !pb14_enabled;  // 取反使能状态
            } else {
                // PD4按键模式下：PC13按键切换PB7 LED的使能状态
                pb7_enabled = !pb7_enabled;    // 取反使能状态
            }
        }
        
        // 检测PD4按键（模式切换）
        if (check_pd4_button()) {
            mode = !mode;  // 切换工作模式（0↔1）
            if (mode == 1) {
                // 进入PD4按键模式：PD4 LED不参与跑马灯
                pd4_enabled = 0;   // 禁用PD4参与跑马灯
                led_off(GPIOD, 4); // 立即熄灭PD4 LED
            } else {
                // 返回正常模式：PD4 LED参与跑马灯
                pd4_enabled = 1;   // 启用PD4参与跑马灯
            }
        }
        
        //跑马灯循环开始
        
        // 第一步：控制PD4 LED（LED1）根据使能状态决定是否点亮
        if (pd4_enabled) {
            led_on(GPIOD, 4);  // 点亮PD4 LED
        } else {
            led_off(GPIOD, 4); // 熄灭PD4 LED
        }
        delay_ms(300);  // 保持当前状态300毫秒
        
        // 第二步：控制PB14 LED（LED2）- 根据使能状态决定是否点亮
        if (pb14_enabled) {
            led_on(GPIOB, 14);  // 点亮PB14 LED
        } else {
            led_off(GPIOB, 14); // 熄灭PB14 LED
        }
        delay_ms(300);  // 保持当前状态300毫秒
        
        // 第三步：控制PB7 LED（LED3）- 根据使能状态决定是否点亮
        // 符合任务要求：使用自定义宏控制PB7 LED
        if (pb7_enabled) {
            PB7_LED_ON();   // 使用宏点亮PB7 LED
        } else {
            PB7_LED_OFF();  // 使用宏熄灭PB7 LED
        }
        delay_ms(300);  // 保持当前状态300毫秒
        
        // 第四步：控制PB0 LED（LED4）- 始终参与跑马灯
        // 符合任务要求：使用直接内存访问控制PB0 LED
        pb0_led_on();   // 使用直接内存访问点亮PB0 LED
        delay_ms(300);  // 保持当前状态300毫秒
        
        // 全部熄灭阶段：关闭所有LED，形成跑马灯效果
        led_off(GPIOD, 4);   // 熄灭PD4 LED
        led_off(GPIOB, 14);  // 熄灭PB14 LED
        PB7_LED_OFF();       // 使用宏熄灭PB7 LED
        pb0_led_off();       // 使用直接内存访问熄灭PB0 LED
        delay_ms(300);  // 保持全部熄灭状态300毫秒
        
        // 循环回到开始，继续检测按键和运行跑马灯
    }
}