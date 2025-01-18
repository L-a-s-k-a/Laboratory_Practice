#include "init.h"

// 系统时钟初始化函数
void RCC_Ini(void)
{
        // 清除 RCC 寄存器的设置
        MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
        CLEAR_REG(RCC->CFGR);
        while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET) ;  // 等待时钟切换完成
        CLEAR_BIT(RCC->CR, RCC_CR_PLLON);  // 禁用 PLL
        while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);  // 等待 PLL 准备好
        CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);  // 禁用外部晶振和时钟检测
        while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);  // 等待外部晶振准备好
        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);  // 禁用外部晶振旁路

        // 启用外部晶振
        SET_BIT(RCC->CR, RCC_CR_HSEON); 
        while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);  // 等待外部晶振启动完成
        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); // 清除旁路设置
        SET_BIT(RCC->CR, RCC_CR_CSSON);    // 启用时钟检测

        // 配置 PLL
        CLEAR_REG(RCC->PLLCFGR);
        SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);  // 选择 HSE 为 PLL 输入源
        MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2);  // 设置 PLL 输入频率分频
        MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7);  // 设置 PLL 倍频
        CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk);  // 设置 PLL 输出频率分频
        SET_BIT(RCC->CR, RCC_CR_PLLON);  // 启用 PLL
        while (READ_BIT(RCC->CR, RCC_CR_PLLRDY));  // 等待 PLL 启动完成

        // 配置系统时钟为 PLL 输出
        MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); 
        MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);  // AHB 时钟分频
        MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);  // APB1 时钟分频
        MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);  // APB2 时钟分频

        // 配置外部时钟输出
        MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk);
        CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);  // 设置 MCO2 输出为系统时钟

        // 配置 Flash 延迟
        MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);  // 设置 Flash 延迟为 5 个 CPU 周期
}

// GPIO 初始化函数
void GPIO_Ini(void)
{
    // 启用 GPIOB 和 GPIOC 时钟
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // 配置板载 LED 引脚为输出模式（PB0, PB7, PB14）
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER0_0);    // PB0
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0); // 设置为高速模式

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER7_0);    // PB7
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7); // 设置为高速模式

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER14_0);   // PB14
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14); // 设置为高速模式

    // 配置外接 LED 引脚（PB15, PB13, PB12）为输出模式
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER15_0);   // PB15
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR15); // 设置为高速模式

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER13_0);   // PB13
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR13); // 设置为高速模式

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER12_0);   // PB12
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR12); // 设置为高速模式

    // 配置 GPIOC 引脚为上拉输入模式（PC0, PC9, PC12）
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0_1);  // PC0 上拉输入
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR9_1);  // PC9 上拉输入
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR12_1); // PC12 上拉输入
}

// 外部中断初始化函数
void ITR_Ini(void)
{
        // 启用 SYSCFG 时钟
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

        // 配置外部中断线0 (EXTI0) 为 PC13 引脚
        MODIFY_REG(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0_Msk, SYSCFG_EXTICR1_EXTI0_PC);  
        SET_BIT(EXTI->IMR, EXTI_IMR_MR0);    // 使能 EXTI0 中断
        SET_BIT(EXTI->RTSR, EXTI_RTSR_TR0);  // 配置为上升沿触发
        SET_BIT(EXTI->FTSR, EXTI_FTSR_TR0);  // 配置为下降沿触发
        NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); // 设置优先级
        NVIC_EnableIRQ(EXTI0_IRQn);  // 使能 EXTI0 中断

        // 配置外部中断线9 (EXTI9) 为 PC9 引脚
        MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI9_Msk, SYSCFG_EXTICR3_EXTI9_PC);
        SET_BIT(EXTI->IMR, EXTI_IMR_MR9);  // 使能 EXTI9 中断
        SET_BIT(EXTI->RTSR, EXTI_RTSR_TR9);  // 配置为上升沿触发
        SET_BIT(EXTI->FTSR, EXTI_FTSR_TR9);  // 配置为下降沿触发
        NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(EXTI9_5_IRQn);  // 使能 EXTI9 中断

        // 配置外部中断线12 (EXTI12) 为 PC12 引脚
        MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI12_Msk, SYSCFG_EXTICR4_EXTI12_PC);
        SET_BIT(EXTI->IMR, EXTI_IMR_MR12);  // 使能 EXTI12 中断
        SET_BIT(EXTI->RTSR, EXTI_RTSR_TR12);  // 配置为上升沿触发
        SET_BIT(EXTI->FTSR, EXTI_FTSR_TR12);  // 配置为下降沿触发
        NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(EXTI15_10_IRQn);  // 使能 EXTI15_10 中断
}

// 系统定时器初始化函数
void SysTick_Init(void)
{
        CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);  // 禁用系统定时器
        SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);  // 启用定时器中断
        SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);  // 使用 AHB 时钟源
        MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 179999 << SysTick_LOAD_RELOAD_Pos); // 配置定时器为 1kHz
        MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 179999 << SysTick_VAL_CURRENT_Pos); // 设置初始值
        SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);  // 启用定时器
}
