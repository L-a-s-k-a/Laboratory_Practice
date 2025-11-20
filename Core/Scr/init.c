#include "init.h"

// 全局变量定义
volatile uint8_t g_pc8_mode = 0;  // PC8模式：0=输入模式，1=输出模式

// 按钮标志(用于外部分析)
volatile uint8_t button1_flag = 0;  // PC12按钮标志
volatile uint8_t button2_flag = 0;  // PC6按钮标志  
volatile uint8_t button3_flag = 0;  // PC8按钮标志

// LED状态变量(0:灭，1:亮)
volatile uint8_t led_1_state = 0;  // 红色LED状态 (PB14)
volatile uint8_t led_2_state = 0;  // 蓝色LED状态 (PB7)
volatile uint8_t led_3_state = 0;  // 黄色LED状态 (PB0)
volatile uint8_t led_4_state = 0;  // 外部LED状态 (PC8输出时)

// GPIO初始化函数 - 直接操作寄存器方式
void GPIO_Init_Memory(void) {
    // 第一步：启用GPIO时钟 
    volatile uint32_t *rcc_ahb1enr = (volatile uint32_t *)(RCC_BASE_ADDR + AHB1ENR_OFFSET);
    // 设置GPIOB、GPIOC的时钟使能位（bit1、bit2）
    *rcc_ahb1enr |= (1 << 1) | (1 << 2);  // 使能GPIOB、GPIOC时钟
    
    // 第二步：配置PC12、PC6为输入模式+内部上拉
    volatile uint32_t *pc12_moder = (volatile uint32_t *)(GPIOC_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pc12_pupdr = (volatile uint32_t *)(GPIOC_BASE_ADDR + PUPDR_OFFSET);
    volatile uint32_t *pc6_moder = (volatile uint32_t *)(GPIOC_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pc6_pupdr = (volatile uint32_t *)(GPIOC_BASE_ADDR + PUPDR_OFFSET);
    
    // 配置PC12为输入模式+内部上拉
    *pc12_moder &= ~(3 << (12 * 2));     // 设置PC12为输入模式(00)
    *pc12_pupdr = (*pc12_pupdr & ~(3 << (12 * 2))) | (1 << (12 * 2));  // 设置PC12内部上拉电阻(01)
    
    // 配置PC6为输入模式+内部上拉
    *pc6_moder &= ~(3 << (6 * 2));       // 设置PC6为输入模式(00)
    *pc6_pupdr = (*pc6_pupdr & ~(3 << (6 * 2))) | (1 << (6 * 2));      // 设置PC6内部上拉电阻(01)
    
    // 第三步：初始配置PC8为输入模式+内部上拉（按键输入模式）
    PC8_Set_Input_PullUp();
    
    // 第四步：配置PB14 LED
    volatile uint32_t *pb14_moder = (volatile uint32_t *)(GPIOB_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pb14_otyper = (volatile uint32_t *)(GPIOB_BASE_ADDR + OTYPER_OFFSET);
    volatile uint32_t *pb14_ospeedr = (volatile uint32_t *)(GPIOB_BASE_ADDR + OSPEEDR_OFFSET);
    volatile uint32_t *pb14_pupdr = (volatile uint32_t *)(GPIOB_BASE_ADDR + PUPDR_OFFSET);
    
    *pb14_moder = (*pb14_moder & ~(3 << (14 * 2))) | (1 << (14 * 2));      // 设置PB14为输出模式(01)
    *pb14_otyper &= ~(1 << 14);                                            // 设置PB14为推挽输出(0)
    *pb14_ospeedr = (*pb14_ospeedr & ~(3 << (14 * 2))) | (2 << (14 * 2));  // 设置PB14为高速输出(10)
    *pb14_pupdr &= ~(3 << (14 * 2));                                       // 禁用PB14内部上下拉电阻(00)
    led_off(GPIOB, 14);                                                    // PB14输出低电平（LED熄灭）
    led_1_state = 0;                                                       // 更新红色LED状态
    
    // 第五步：配置PB7 LED（蓝色）
    PB7_MODER_OUTPUT();        // 设置PB7为输出模式
    PB7_OTYPER_PUSHPULL();     // 设置PB7为推挽输出
    PB7_OSPEEDR_HIGH();        // 设置PB7为高速模式
    PB7_PUPDR_NOPULL();        // 禁用PB7内部上下拉电阻
    PB7_LED_OFF();             // PB7输出低电平（LED熄灭）
    led_2_state = 0;           // 更新蓝色LED状态
    
    // 第六步：配置PB0 LED（黄色）
    volatile uint32_t *pb0_moder = (volatile uint32_t *)(GPIOB_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pb0_otyper = (volatile uint32_t *)(GPIOB_BASE_ADDR + OTYPER_OFFSET);
    volatile uint32_t *pb0_ospeedr = (volatile uint32_t *)(GPIOB_BASE_ADDR + OSPEEDR_OFFSET);
    volatile uint32_t *pb0_pupdr = (volatile uint32_t *)(GPIOB_BASE_ADDR + PUPDR_OFFSET);
    
    *pb0_moder = (*pb0_moder & ~(3 << (0 * 2))) | (1 << (0 * 2));      // 设置PB0为输出模式(01)
    *pb0_otyper &= ~(1 << 0);                                          // 设置PB0为推挽输出(0)
    *pb0_ospeedr = (*pb0_ospeedr & ~(3 << (0 * 2))) | (2 << (0 * 2));  // 设置PB0为高速输出(10)
    *pb0_pupdr &= ~(3 << (0 * 2));                                     // 禁用PB0内部上下拉电阻(00)
    pb0_led_off();                                                     // PB0输出低电平（LED熄灭）
    led_3_state = 0;                                                   // 更新黄色LED状态
    
    // 第七步：配置PB8 LED
    PB8_MODER_OUTPUT();        // 设置PB8为输出模式
    PB8_OTYPER_PUSHPULL();     // 设置PB8为推挽输出
    PB8_OSPEEDR_HIGH();        // 设置PB8为高速模式
    PB8_PUPDR_NOPULL();        // 禁用PB8内部上下拉电阻
    PB8_LED_OFF();             // PB8输出低电平（LED熄灭）
    
    // 初始化外部LED状态
    led_4_state = 0;           // 外部LED初始状态为熄灭
}

/**
 * 设置PC8为输入模式+内部上拉（按键输入模式）
 */
void PC8_Set_Input_PullUp(void) {
    volatile uint32_t *pc8_moder = (volatile uint32_t *)(GPIOC_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pc8_pupdr = (volatile uint32_t *)(GPIOC_BASE_ADDR + PUPDR_OFFSET);
    
    // 配置PC8引脚模式：设置为输入模式(00)
    *pc8_moder &= ~(3 << (8 * 2));
    // 配置PC8上拉下拉：设置为上拉模式(01)
    *pc8_pupdr = (*pc8_pupdr & ~(3 << (8 * 2))) | (1 << (8 * 2));
    
    // 更新PC8模式状态
    g_pc8_mode = 0;  // 输入模式
    led_4_state = 0; // 外部LED状态更新为熄灭
}

/**
 * 设置PC8为输出模式+高电平
 */
void PC8_Set_Output_High(void) {
    volatile uint32_t *pc8_moder = (volatile uint32_t *)(GPIOC_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pc8_otyper = (volatile uint32_t *)(GPIOC_BASE_ADDR + OTYPER_OFFSET);
    volatile uint32_t *pc8_ospeedr = (volatile uint32_t *)(GPIOC_BASE_ADDR + OSPEEDR_OFFSET);
    volatile uint32_t *pc8_pupdr = (volatile uint32_t *)(GPIOC_BASE_ADDR + PUPDR_OFFSET);
    volatile uint32_t *pc8_bsrr = (volatile uint32_t *)(GPIOC_BASE_ADDR + BSRR_OFFSET);
    
    // 配置PC8引脚模式：设置为输出模式(01)
    *pc8_moder = (*pc8_moder & ~(3 << (8 * 2))) | (1 << (8 * 2));
    // 配置PC8输出类型：设置为推挽输出(0)
    *pc8_otyper &= ~(1 << 8);
    // 配置PC8输出速度：设置为高速模式(10)
    *pc8_ospeedr = (*pc8_ospeedr & ~(3 << (8 * 2))) | (2 << (8 * 2));
    // 配置PC8上拉下拉：设置为无上下拉(00)
    *pc8_pupdr &= ~(3 << (8 * 2));
    // 设置PC8输出高电平
    *pc8_bsrr = (1U << 8);
    
    // 更新PC8模式状态
    g_pc8_mode = 1;    // 输出模式
    led_4_state = 1;   // 外部LED状态更新为点亮
}
