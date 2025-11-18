#include "init.h"  
// GPIO初始化函数 - 直接操作寄存器方式
// 功能：配置所有使用的GPIO引脚，包括LED和按键
void GPIO_Init_Memory(void){
    //第一步：启用GPIO时钟 
    // 获取RCC AHB1外设时钟使能寄存器的内存地址
    volatile uint32_t *rcc_ahb1enr = (volatile uint32_t *)(RCC_BASE_ADDR + AHB1ENR_OFFSET);
    // 设置GPIOB、GPIOC、GPIOD的时钟使能位（bit1、bit2、bit3）
    *rcc_ahb1enr |= (1 << 1) | (1 << 2) | (1 << 3);  // 使能三个GPIO端口的时钟
    
    //第二步：配置PD4 (LED1 + 按键)
    // 获取PD4相关寄存器的内存地址
    volatile uint32_t *pd4_moder = (volatile uint32_t *)(GPIOD_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pd4_otyper = (volatile uint32_t *)(GPIOD_BASE_ADDR + OTYPER_OFFSET);
    volatile uint32_t *pd4_ospeedr = (volatile uint32_t *)(GPIOD_BASE_ADDR + OSPEEDR_OFFSET);
    volatile uint32_t *pd4_pupdr = (volatile uint32_t *)(GPIOD_BASE_ADDR + PUPDR_OFFSET);
    // 配置PD4引脚模式：先清除原来的设置，再设置为输出模式(01)
    *pd4_moder = (*pd4_moder & ~(3 << (4 * 2))) | (1 << (4 * 2));      // 设置PD4为通用输出模式
    // 配置PD4输出类型：清除第4位，设置为推挽输出(0)
    *pd4_otyper &= ~(1 << 4);                                          // 设置PD4为推挽输出
    // 配置PD4输出速度：先清除原来的设置，再设置为高速模式(10)
    *pd4_ospeedr = (*pd4_ospeedr & ~(3 << (4 * 2))) | (2 << (4 * 2));  // 设置PD4为高速输出
    // 配置PD4上拉下拉：先清除原来的设置，再设置为上拉模式(01)
    *pd4_pupdr = (*pd4_pupdr & ~(3 << (4 * 2))) | (1 << (4 * 2));      // 设置PD4内部上拉电阻
    
    //第三步：配置PB14 (LED2)
    // 获取PB14相关寄存器的内存地址
    volatile uint32_t *pb14_moder = (volatile uint32_t *)(GPIOB_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pb14_otyper = (volatile uint32_t *)(GPIOB_BASE_ADDR + OTYPER_OFFSET);
    volatile uint32_t *pb14_ospeedr = (volatile uint32_t *)(GPIOB_BASE_ADDR + OSPEEDR_OFFSET);
    volatile uint32_t *pb14_pupdr = (volatile uint32_t *)(GPIOB_BASE_ADDR + PUPDR_OFFSET);
    
    // 配置PB14引脚模式：先清除原来的设置，再设置为输出模式(01)
    *pb14_moder = (*pb14_moder & ~(3 << (14 * 2))) | (1 << (14 * 2));      // 设置PB14为通用输出模式
    // 配置PB14输出类型：清除第14位，设置为推挽输出(0)
    *pb14_otyper &= ~(1 << 14);                                            // 设置PB14为推挽输出
    // 配置PB14输出速度：先清除原来的设置，再设置为高速模式(10)
    *pb14_ospeedr = (*pb14_ospeedr & ~(3 << (14 * 2))) | (2 << (14 * 2));  // 设置PB14为高速输出
    // 配置PB14上拉下拉：清除第14位的上下拉设置，设置为无上下拉(00)
    *pb14_pupdr &= ~(3 << (14 * 2));                                       // 禁用PB14内部上下拉电阻
    
    // 第四步：配置PB7 (LED3)使用自定义宏
    // 使用头文件中定义的自定义宏来配置PB7引脚
    PB7_MODER_OUTPUT();        // 宏：设置PB7为输出模式
    PB7_OTYPER_PUSHPULL();     // 宏：设置PB7为推挽输出
    PB7_OSPEEDR_HIGH();        // 宏：设置PB7为高速模式
    PB7_PUPDR_NOPULL();        // 宏：禁用PB7内部上下拉电阻
    
    // 第五步：配置PB0 (LED4) - 使用真正的直接内存访问
    // 获取PB0相关寄存器的内存地址 - 使用硬编码基地址+偏移量的方式
    volatile uint32_t *pb0_moder = (volatile uint32_t *)(GPIOB_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pb0_otyper = (volatile uint32_t *)(GPIOB_BASE_ADDR + OTYPER_OFFSET);
    volatile uint32_t *pb0_ospeedr = (volatile uint32_t *)(GPIOB_BASE_ADDR + OSPEEDR_OFFSET);
    volatile uint32_t *pb0_pupdr = (volatile uint32_t *)(GPIOB_BASE_ADDR + PUPDR_OFFSET);
    
    // 直接内存访问配置PB0 - 不依赖任何库函数，直接操作寄存器
    // 配置PB0引脚模式：先清除原来的设置，再设置为输出模式(01)
    *pb0_moder = (*pb0_moder & ~(3 << (0 * 2))) | (1 << (0 * 2));      // 设置PB0为通用输出模式
    // 配置PB0输出类型：清除第0位，设置为推挽输出(0)
    *pb0_otyper &= ~(1 << 0);                                          // 设置PB0为推挽输出
    // 配置PB0输出速度：先清除原来的设置，再设置为高速模式(10)
    *pb0_ospeedr = (*pb0_ospeedr & ~(3 << (0 * 2))) | (2 << (0 * 2));  // 设置PB0为高速输出
    // 配置PB0上拉下拉：清除第0位的上下拉设置，设置为无上下拉(00)
    *pb0_pupdr &= ~(3 << (0 * 2));                                     // 禁用PB0内部上下拉电阻
    
    //第六步：配置PC13 (按键引脚)
    // 获取PC13相关寄存器的内存地址
    volatile uint32_t *pc13_moder = (volatile uint32_t *)(GPIOC_BASE_ADDR + MODER_OFFSET);
    volatile uint32_t *pc13_pupdr = (volatile uint32_t *)(GPIOC_BASE_ADDR + PUPDR_OFFSET);
    // 配置PC13引脚模式：清除原来的设置，设置为输入模式(00)
    *pc13_moder &= ~(3 << (13 * 2));       // 设置PC13为输入模式
    // 配置PC13上拉下拉：先清除原来的设置，再设置为下拉模式(10)
    *pc13_pupdr = (*pc13_pupdr & ~(3 << (13 * 2))) | (2 << (13 * 2));  // 设置PC13内部下拉电阻
}
