#include "init.h" 

// void GPIO_Ini_1(void){
//     SET_BIT(RCC_GPIO_EN, RCC_GPIOB_EN | RCC_GPIOC_EN); // 开启GPIO B, GPIO C
//     SET_BIT(GPIOB_MODER, GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN0_OUT | GPIOB_MODE_PIN14_OUT | GPIOB_MODE_PIN8_OUT | GPIOB_MODE_PIN9_OUT | GPIOB_MODE_PIN10_OUT); // 设置为输出模式
//     SET_BIT(GPIOB_OTYPER, GPIOB_OTYPE_PIN7_PP | GPIOB_OTYPE_PIN0_PP | GPIOB_OTYPE_PIN14_PP | GPIOB_OTYPE_PIN8_PP | GPIOB_OTYPE_PIN9_PP | GPIOB_OTYPE_PIN10_PP); // 设置为推挽输出
//     SET_BIT(GPIOB_OSPEEDR, GPIOB_OSPEED_PIN7_MID | GPIOB_OSPEED_PIN0_MID | GPIOB_OSPEED_PIN14_MID | GPIOB_OSPEED_PIN8_MID | GPIOB_OSPEED_PIN9_MID | GPIOB_OSPEED_PIN10_MID); // 设置速度为中等
//     SET_BIT(GPIOB_PUPDR, GPIOB_PUPDR_PIN7_NOPUPD | GPIOB_PUPDR_PIN0_NOPUPD | GPIOB_PUPDR_PIN14_NOPUPD | GPIOB_PUPDR_PIN8_NOPUPD | GPIOB_PUPDR_PIN9_NOPUPD | GPIOB_PUPDR_PIN10_NOPUPD); // 禁用上拉/下拉电阻
 
// }

void GPIO_Ini(void) {
    // 启用GPIOC和GPIOB的时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // 启用GPIOC时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // 启用GPIOB时钟

    // 配置GPIOC第13引脚为输入模式（按钮）
    GPIOC->MODER &= ~(3 << (13 * 2)); // 清除模式位
    GPIOC->PUPDR |= (2 << (13 * 2)); // 上拉模式

    // 配置GPIOB引脚为输出模式（LED）
    GPIOB->MODER |= (1 << (0 * 2)); // GPIOB第0引脚为输出模式
    GPIOB->MODER |= (1 << (7 * 2)); // GPIOB第7引脚为输出模式
    GPIOB->MODER |= (1 << (14 * 2)); // GPIOB第14引脚为输出模式
    GPIOB->MODER |= (1 << (8 * 2)); // GPIOB第8引脚为输出模式
    GPIOB->MODER |= (1 << (9 * 2)); // GPIOB第9引脚为输出模式
    GPIOB->MODER |= (1 << (10 * 2)); // GPIOB第10引脚为输出模式
}

void EXTI_Ini(void) {
    // 启用SYSCFG时钟
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // 配置外部中断线路
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; // 将PC13映射到EXTI13

    // 配置EXTI13为上升沿触发和下降沿触发
    EXTI->IMR |= EXTI_IMR_IM13; // 使能中断
    EXTI->FTSR |= EXTI_FTSR_TR13; // 使能下降沿触发
    EXTI->RTSR |= EXTI_RTSR_TR13; // 使能上升沿触发

    // 配置NVIC
    NVIC_SetPriority(EXTI15_10_IRQn, 2); // 设置中断优先级
    NVIC_EnableIRQ(EXTI15_10_IRQn); // 使能外部中断
}

void SysTick_Ini(void) {
    // 设置SysTick计数频率为1ms
    SysTick->LOAD = 16000 - 1; // 假设系统时钟为16MHz
    SysTick->VAL = 0; // 清零当前计数值
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | // 使用处理器时钟
                    SysTick_CTRL_TICKINT_Msk | // 使能中断
                    SysTick_CTRL_ENABLE_Msk; // 启动SysTick
}



// #include "../Inc/init.h"


// void RCC_Init(void)
// {
//     // 启用 GPIOB 时钟
//     RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // 使能 GPIOB 时钟
//     RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // 使能系统配置控制器时钟
//     RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // 使能定时器2时钟
// }

// void GPIO_Ini(void)
// {
//     // 设置 PB0, PB7, PB14, PB8, PB9, PB10 为输出模式
//     GPIOB_MODER |= (1 << (0 * 2)) | (1 << (7 * 2)) | (1 << (14 * 2)) | (1 << (8 * 2)) | (1 << (9 * 2)) | (1 << (10 * 2)); // 0b01

//     // 设置 PB0, PB7, PB14, PB8, PB9, PB10 为推挽输出
//     GPIOB_OTYPER &= ~((1 << 0) | (1 << 7) | (1 << 14) | (1 << 8) | (1 << 9) | (1 << 10)); // 0b0

//     // 设置 PB0, PB7, PB14, PB8, PB9, PB10 为低速模式
//     GPIOB_OSPEEDR &= ~((3 << (0 * 2)) | (3 << (7 * 2)) | (3 << (14 * 2)) | (3 << (8 * 2)) | (3 << (9 * 2)) | (3 << (10 * 2))); // 0b00

//     // 设置 PB0, PB7, PB14, PB8, PB9, PB10 为无上拉下拉
//     GPIOB_PUPDR &= ~((3 << (0 * 2)) | (3 << (7 * 2)) | (3 << (14 * 2)) | (3 << (8 * 2)) | (3 << (9 * 2)) | (3 << (10 * 2))); // 0b00

//     // 配置按钮 PC13 为输入
//     GPIOC->MODER &= ~(3 << (13 * 2)); // 输入模式
//     SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI13_PC; // 选择 PC13 作为 EXTI13 的输入
//     EXTI->IMR |= EXTI_IMR_MR13; // 使能 EXTI13 中断
//     EXTI->FTSR |= EXTI_FTSR_TR13; // 下降沿触发中断
// }

// void ITR_init(void)
// {
//     NVIC_EnableIRQ(EXTI15_10_IRQn); // 使能 EXTI15_10 中断
//     NVIC_SetPriority(EXTI15_10_IRQn, 2); // 设置中断优先级
// }

// void SysTick_Init(void)
// {
//     SysTick->LOAD = 16000 - 1; // 1ms @ 16MHz
//     SysTick->VAL = 0; // 清零计数器
//     SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; // 启用 SysTick
// }


