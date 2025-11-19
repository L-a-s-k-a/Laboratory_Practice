#ifndef INIT_H
#define INIT_H

#include "E:/VSCode-file/Laboratory_Practice/CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "E:/VSCode-file/Laboratory_Practice/CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h"

// 使用CMSIS提供的地址定义，这里只定义寄存器偏移量
#define MODER_OFFSET       0x00
#define OTYPER_OFFSET      0x04
#define OSPEEDR_OFFSET     0x08
#define PUPDR_OFFSET       0x0C
#define IDR_OFFSET         0x10
#define BSRR_OFFSET        0x18
#define AHB1ENR_OFFSET     0x30

// 引脚定义
#define ONBOARD_GREEN_PIN   0   // PB0
#define ONBOARD_BLUE_PIN    7   // PB7
#define ONBOARD_RED_PIN     14  // PB14
#define EXTERNAL_GREEN_PIN  8   // PB8
#define EXTERNAL_BLUE_PIN   9   // PB9
#define EXTERNAL_RED_PIN    10  // PB10

#define KEY_MODE_PIN        3   // PG3
#define KEY_CTRL1_PIN       6   // PC6
#define KEY_CTRL2_PIN       8   // PC8
#define KEY_CTRL3_PIN       9   // PC9

// ====== 外接LED控制宏 - 纯地址级直接内存访问 ======
#define EXTERNAL_GREEN_MODER_OUT()    *((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) = \
                                     (*((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) & ~(3U << (EXTERNAL_GREEN_PIN * 2))) | (1U << (EXTERNAL_GREEN_PIN * 2))

#define EXTERNAL_GREEN_OTYPER_PP()    *((volatile uint32_t*)(GPIOB_BASE + OTYPER_OFFSET)) &= ~(1U << EXTERNAL_GREEN_PIN)

#define EXTERNAL_GREEN_OSPEEDR_HIGH() *((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) = \
                                     (*((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) & ~(3U << (EXTERNAL_GREEN_PIN * 2))) | (2U << (EXTERNAL_GREEN_PIN * 2))

#define EXTERNAL_GREEN_PUPDR_NOPULL() *((volatile uint32_t*)(GPIOB_BASE + PUPDR_OFFSET)) &= ~(3U << (EXTERNAL_GREEN_PIN * 2))

#define EXTERNAL_GREEN_ON()           *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << EXTERNAL_GREEN_PIN)
#define EXTERNAL_GREEN_OFF()          *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << (EXTERNAL_GREEN_PIN + 16U))

// 蓝色外接LED宏
#define EXTERNAL_BLUE_MODER_OUT()     *((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) = \
                                     (*((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) & ~(3U << (EXTERNAL_BLUE_PIN * 2))) | (1U << (EXTERNAL_BLUE_PIN * 2))

#define EXTERNAL_BLUE_OTYPER_PP()     *((volatile uint32_t*)(GPIOB_BASE + OTYPER_OFFSET)) &= ~(1U << EXTERNAL_BLUE_PIN)

#define EXTERNAL_BLUE_OSPEEDR_HIGH()  *((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) = \
                                     (*((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) & ~(3U << (EXTERNAL_BLUE_PIN * 2))) | (2U << (EXTERNAL_BLUE_PIN * 2))

#define EXTERNAL_BLUE_PUPDR_NOPULL()  *((volatile uint32_t*)(GPIOB_BASE + PUPDR_OFFSET)) &= ~(3U << (EXTERNAL_BLUE_PIN * 2))

#define EXTERNAL_BLUE_ON()            *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << EXTERNAL_BLUE_PIN)
#define EXTERNAL_BLUE_OFF()           *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << (EXTERNAL_BLUE_PIN + 16U))

// 红色外接LED宏
#define EXTERNAL_RED_MODER_OUT()      *((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) = \
                                     (*((volatile uint32_t*)(GPIOB_BASE + MODER_OFFSET)) & ~(3U << (EXTERNAL_RED_PIN * 2))) | (1U << (EXTERNAL_RED_PIN * 2))

#define EXTERNAL_RED_OTYPER_PP()      *((volatile uint32_t*)(GPIOB_BASE + OTYPER_OFFSET)) &= ~(1U << EXTERNAL_RED_PIN)

#define EXTERNAL_RED_OSPEEDR_HIGH()   *((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) = \
                                     (*((volatile uint32_t*)(GPIOB_BASE + OSPEEDR_OFFSET)) & ~(3U << (EXTERNAL_RED_PIN * 2))) | (2U << (EXTERNAL_RED_PIN * 2))

#define EXTERNAL_RED_PUPDR_NOPULL()   *((volatile uint32_t*)(GPIOB_BASE + PUPDR_OFFSET)) &= ~(3U << (EXTERNAL_RED_PIN * 2))

#define EXTERNAL_RED_ON()             *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << EXTERNAL_RED_PIN)
#define EXTERNAL_RED_OFF()            *((volatile uint32_t*)(GPIOB_BASE + BSRR_OFFSET)) = (1U << (EXTERNAL_RED_PIN + 16U))

// ====== STMViewer监测变量结构体 ======
typedef struct {
    // 系统状态
    volatile uint8_t current_mode;           // 当前模式 (1-3)
    volatile uint32_t pg3_press_count;       // PG3按键按下次数
    
    // LED状态 - 便于STMViewer显示波形
    volatile uint8_t onboard_green_state;    // 板载绿色LED (1=亮, 0=灭)
    volatile uint8_t onboard_blue_state;     // 板载蓝色LED (1=亮, 0=灭)
    volatile uint8_t onboard_red_state;      // 板载红色LED (1=亮, 0=灭)
    volatile uint8_t external_green_state;   // 外接绿色LED (1=亮, 0=灭)
    volatile uint8_t external_blue_state;    // 外接蓝色LED (1=亮, 0=灭)
    volatile uint8_t external_red_state;     // 外接红色LED (1=亮, 0=灭)
    
    // 按键状态 - 符合内部上拉逻辑 (1=未按下, 0=按下)
    volatile uint8_t pg3_key_state;          // PG3按键状态 (1=未按下, 0=按下)
    volatile uint8_t pc6_key_state;          // PC6按键状态 (1=未按下, 0=按下)
    volatile uint8_t pc8_key_state;          // PC8按键状态 (1=未按下, 0=按下)
    volatile uint8_t pc9_key_state;          // PC9按键状态 (1=未按下, 0=按下)
    
    // 按键有效按下标志 - 用于调试
    volatile uint8_t pg3_pressed_flag;       // PG3有效按下标志 (脉冲信号)
    volatile uint8_t pc6_pressed_flag;       // PC6有效按下标志
    volatile uint8_t pc8_pressed_flag;       // PC8有效按下标志
    volatile uint8_t pc9_pressed_flag;       // PC9有效按下标志
    
    // 调试信息
    volatile uint32_t loop_counter;          // 主循环计数器
    volatile uint32_t last_mode_change;      // 上次模式切换时间戳
    
    // 新增：用于脉冲控制的计数器
    volatile uint8_t pg3_pulse_counter;      // PG3脉冲控制计数器
    
} SystemMonitor_t;

// 全局监测变量声明
extern SystemMonitor_t g_system_monitor;

// 函数声明
void GPIO_Init_Memory(void);
void Green_LEDs_On(void);
void Green_LEDs_Off(void);
void Blue_LEDs_On(void);
void Blue_LEDs_Off(void);
void Red_LEDs_On(void);
void Red_LEDs_Off(void);
void All_LEDs_Off(void);
void Switch_Mode(void);
uint32_t Get_PG3_Press_Count(void);

#endif /* INIT_H */