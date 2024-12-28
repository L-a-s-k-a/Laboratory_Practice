#include "init.h"

void RCC_Ini(void)
{
    // [1] 清理 RCC 寄存器 (内部HSI,PLL等复位)
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

    // [2] 启用外部时钟HSE
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
    SET_BIT(RCC->CR, RCC_CR_CSSON);

    // [3] 配置PLL => 得到180MHz
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    // 将 HSE(8MHz或16MHz) 先分频到2MHz，再×180，再除2 => 180MHz
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); 
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk,
               RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 |
               RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7 );
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk);
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);

    // [4] 选择PLL做System Clock & 配置分频
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE,  RCC_CFGR_HPRE_DIV1);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);

    // [5] Flash 等待周期设置
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}

void GPIO_Ini(void)
{
    // 打开GPIOB、GPIOC时钟
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // [LED引脚: PB0,1,2,6,7,14 => output]
    // 这里示例直接把 PB0/1/2/6/7/14 都设为推挽输出
    SET_BIT(GPIOB->MODER, 
            GPIO_MODER_MODE0_0  | GPIO_MODER_MODE1_0  |
            GPIO_MODER_MODE2_0  | GPIO_MODER_MODE6_0  |
            GPIO_MODER_MODE7_0  | GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, 
            GPIO_OTYPER_OT0  | GPIO_OTYPER_OT1  |
            GPIO_OTYPER_OT2  | GPIO_OTYPER_OT6  |
            GPIO_OTYPER_OT7  | GPIO_OTYPER_OT14);
    // 速度设中等
    SET_BIT(GPIOB->OSPEEDR,
            GPIO_OSPEEDER_OSPEEDR0_0  | GPIO_OSPEEDER_OSPEEDR1_0  |
            GPIO_OSPEEDER_OSPEEDR2_0  | GPIO_OSPEEDER_OSPEEDR6_0  |
            GPIO_OSPEEDER_OSPEEDR7_0  | GPIO_OSPEEDER_OSPEEDR14_0);
    // 无上下拉
    CLEAR_BIT(GPIOB->PUPDR,
            GPIO_PUPDR_PUPDR0  | GPIO_PUPDR_PUPDR1  |
            GPIO_PUPDR_PUPDR2  | GPIO_PUPDR_PUPDR6  |
            GPIO_PUPDR_PUPDR7  | GPIO_PUPDR_PUPDR14 );

    // [按键: PC13, PC6, PC7 => Input 下拉]
    // 说明：板载按键通常接PC13。外部按键接PC6,7。
    //       使用下拉方式(PULL-DOWN)，按下时电平为高
    CLEAR_BIT(GPIOC->MODER, 
              GPIO_MODER_MODE13 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
    SET_BIT(GPIOC->PUPDR, 
            GPIO_PUPDR_PUPD13_1 | GPIO_PUPDR_PUPD6_1 | GPIO_PUPDR_PUPD7_1);
}

void EXTI_ITR_Ini(void)
{
    // 使能 SYSCFG 时钟
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    // ---------------- 按键1: PC13 => EXTI13 ----------------
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, 
               SYSCFG_EXTICR4_EXTI13_PC);  // 这里库里有现成宏
    SET_BIT(EXTI->IMR,  EXTI_IMR_MR13);
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13);  
    CLEAR_BIT(EXTI->FTSR, EXTI_FTSR_TR13);

    // ---------------- 按键2: PC6 => EXTI6 ----------------
    // 对应 SYSCFG->EXTICR[1] (EXTICR2)，bits [27..24]
    // 先清除 0xF << 24，再写入 2 << 24 (表示 PC=2)
    MODIFY_REG(SYSCFG->EXTICR[1],
               (0xFU << 24),   // 要清除的掩码
               (2U << 24));    // 要设置的值 (PC=2)
    
    SET_BIT(EXTI->IMR,  EXTI_IMR_MR6);
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR6);
    CLEAR_BIT(EXTI->FTSR, EXTI_FTSR_TR6);

    // ---------------- 按键3: PC7 => EXTI7 ----------------
    // 同样在 SYSCFG->EXTICR[1]，bits [31..28]
    MODIFY_REG(SYSCFG->EXTICR[1],
               (0xFU << 28),
               (2U << 28));  // PC=2

    SET_BIT(EXTI->IMR,  EXTI_IMR_MR7);
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR7);
    CLEAR_BIT(EXTI->FTSR, EXTI_FTSR_TR7);

    // ---------------- NVIC 优先级 + 使能中断 ----------------
    // EXTI9_5 用于 PC6/7， EXTI15_10 用于 PC13
    NVIC_SetPriority(EXTI9_5_IRQn, 
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    NVIC_SetPriority(EXTI15_10_IRQn, 
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 1));
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void SysTick_Init(void)
{
    // 关闭 SysTick
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);

    // 使能中断
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);

    // 使用 CPU (AHB) 时钟
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);

    // 1ms的节拍 => CPU=180MHz => 180000=>1ms
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, (180000 - 1));
    // 清零计数器
    WRITE_REG(SysTick->VAL, 0U);

    // 开启 SysTick
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}