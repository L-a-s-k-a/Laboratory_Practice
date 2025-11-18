#include "init.h"

// GPIO初始化函数 - 直接操作寄存器方式

void GPIO_Init_Memory(void){

    //第一步：启用GPIO时钟 
    
    volatile uint32_t *rcc_ahb1enr = (volatile uint32_t *)(RCC_BASE_ADDR + AHB1ENR_OFFSET);
    // 设置GPIOB、GPIOD、GPIOG的时钟使能位（bit1、bit3、bit6）
    *rcc_ahb1enr |= (1 << 1) | (1 << 3) | (1 << 6);  // 使能GPIOB、GPIOD、GPIOG时钟
    
    //第二步：配置PD4为输入模式+上拉（正常模式，避免引脚浮空）
    PD4_Set_Input_PullUp();
    
    //第三步：配置PG3为输入模式（按键引脚，启用内部上拉）
    volatile uint32_t *pg3_moder = (volatile uint32_t *)(GPIOG_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pg3_pupdr = (volatile uint32_t *)(GPIOG_BASE_ADDR + PUPDR_OFFSET);
    // 配置PG3引脚模式：清除原来的设置，设置为输入模式(00)
    *pg3_moder &= ~(3 << (3 * 2));       // 设置PG3为输入模式
    // 配置PG3上拉下拉：先清除原来的设置，再设置为上拉模式(01)
    *pg3_pupdr = (*pg3_pupdr & ~(3 << (3 * 2))) | (1 << (3 * 2));  // 设置PG3内部上拉电阻
    
    //第四步：配置PB14 (LED2) - 初始为低电平（LED熄灭）
    volatile uint32_t *pb14_moder = (volatile uint32_t *)(GPIOB_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pb14_otyper = (volatile uint32_t *)(GPIOB_BASE_ADDR + OTYPER_OFFSET);
    volatile uint32_t *pb14_ospeedr = (volatile uint32_t *)(GPIOB_BASE_ADDR + OSPEEDR_OFFSET);
    volatile uint32_t *pb14_pupdr = (volatile uint32_t *)(GPIOB_BASE_ADDR + PUPDR_OFFSET);
    volatile uint32_t *pb14_bsrr = (volatile uint32_t *)(GPIOB_BASE_ADDR + BSRR_OFFSET);
    
    // 配置PB14引脚模式：先清除原来的设置，再设置为输出模式(01)
    *pb14_moder = (*pb14_moder & ~(3 << (14 * 2))) | (1 << (14 * 2));      // 设置PB14为通用输出模式
    // 配置PB14输出类型：清除第14位，设置为推挽输出(0)
    *pb14_otyper &= ~(1 << 14);                                            // 设置PB14为推挽输出
    // 配置PB14输出速度：先清除原来的设置，再设置为高速模式(10)
    *pb14_ospeedr = (*pb14_ospeedr & ~(3 << (14 * 2))) | (2 << (14 * 2));  // 设置PB14为高速输出
    // 配置PB14上拉下拉：清除第14位的上下拉设置，设置为无上下拉(00)
    *pb14_pupdr &= ~(3 << (14 * 2));                                       // 禁用PB14内部上下拉电阻
    // 设置PB14初始输出低电平（LED熄灭）
    *pb14_bsrr = (1U << (14 + 16U));                                       // PB14输出低电平
    
    // 第五步：配置PB7 (LED3)使用自定义宏 - 初始为低电平（LED熄灭）
    PB7_MODER_OUTPUT();        // 宏：设置PB7为输出模式
    PB7_OTYPER_PUSHPULL();     // 宏：设置PB7为推挽输出
    PB7_OSPEEDR_HIGH();        // 宏：设置PB7为高速模式
    PB7_PUPDR_NOPULL();        // 宏：禁用PB7内部上下拉电阻
    PB7_LED_OFF();             // 宏：PB7输出低电平（LED熄灭）
    
    // 第六步：配置PB0 (LED4) - 使用真正的直接内存访问，初始为低电平（LED熄灭）
    volatile uint32_t *pb0_moder = (volatile uint32_t *)(GPIOB_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pb0_otyper = (volatile uint32_t *)(GPIOB_BASE_ADDR + OTYPER_OFFSET);
    volatile uint32_t *pb0_ospeedr = (volatile uint32_t *)(GPIOB_BASE_ADDR + OSPEEDR_OFFSET);
    volatile uint32_t *pb0_pupdr = (volatile uint32_t *)(GPIOB_BASE_ADDR + PUPDR_OFFSET);
    volatile uint32_t *pb0_bsrr = (volatile uint32_t *)(GPIOB_BASE_ADDR + BSRR_OFFSET);
    
    // 直接内存访问配置PB0
    *pb0_moder = (*pb0_moder & ~(3 << (0 * 2))) | (1 << (0 * 2));      // 设置PB0为通用输出模式

    *pb0_otyper &= ~(1 << 0);                                          // 设置PB0为推挽输出

    *pb0_ospeedr = (*pb0_ospeedr & ~(3 << (0 * 2))) | (2 << (0 * 2));  // 设置PB0为高速输出
    
    *pb0_pupdr &= ~(3 << (0 * 2));                                     // 禁用PB0内部上下拉电阻
    // 设置PB0初始输出低电平（LED熄灭）
    *pb0_bsrr = (1U << (0 + 16U));                                     // PB0输出低电平
}


//设置PD4为输入模式+上拉（正常模式，用于按键检测）


void PD4_Set_Input_PullUp(void) {
    volatile uint32_t *pd4_moder = (volatile uint32_t *)(GPIOD_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pd4_pupdr = (volatile uint32_t *)(GPIOD_BASE_ADDR + PUPDR_OFFSET);
    // 配置PD4引脚模式：清除原来的设置，设置为输入模式(00)
    *pd4_moder &= ~(3 << (4 * 2));       // 设置PD4为输入模式
    // 配置PD4上拉下拉：先清除原来的设置，再设置为上拉模式(01)
    *pd4_pupdr = (*pd4_pupdr & ~(3 << (4 * 2))) | (1 << (4 * 2));  // 设置PD4内部上拉电阻
    
    // 更新PD4模式状态
    g_pd4_mode_state = 0;  // 输入上拉模式
}
//设置PD4为输出模式+高电平
void PD4_Set_Output_High(void) {
    volatile uint32_t *pd4_moder = (volatile uint32_t *)(GPIOD_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pd4_otyper = (volatile uint32_t *)(GPIOD_BASE_ADDR + OTYPER_OFFSET);
    volatile uint32_t *pd4_ospeedr = (volatile uint32_t *)(GPIOD_BASE_ADDR + OSPEEDR_OFFSET);
    volatile uint32_t *pd4_pupdr = (volatile uint32_t *)(GPIOD_BASE_ADDR + PUPDR_OFFSET);
    volatile uint32_t *pd4_bsrr = (volatile uint32_t *)(GPIOD_BASE_ADDR + BSRR_OFFSET);
    
    // 配置PD4引脚模式：先清除原来的设置，再设置为输出模式(01)
    *pd4_moder = (*pd4_moder & ~(3 << (4 * 2))) | (1 << (4 * 2));      // 设置PD4为通用输出模式
    // 配置PD4输出类型：清除第4位，设置为推挽输出(0)
    *pd4_otyper &= ~(1 << 4);                                          // 设置PD4为推挽输出
    // 配置PD4输出速度：先清除原来的设置，再设置为高速模式(10)
    *pd4_ospeedr = (*pd4_ospeedr & ~(3 << (4 * 2))) | (2 << (4 * 2));  // 设置PD4为高速输出
    // 配置PD4上拉下拉：清除第4位的上下拉设置，设置为无上下拉(00)
    *pd4_pupdr &= ~(3 << (4 * 2));                                     // 禁用PD4内部上下拉电阻
    // 设置PD4输出高电平
    *pd4_bsrr = (1U << 4);                                             // 设置PD4输出高电平
    
    // 更新PD4模式状态
    g_pd4_mode_state = 1;  // 输出高电平模式//////
}