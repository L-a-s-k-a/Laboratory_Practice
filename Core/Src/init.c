#include "init.h"

//===== 1) SystemClock_Config =====
void SystemClock_Config(void)
{
    // 简化示例：使用 HSI=16MHz，不启用PLL
    RCC->CR |= RCC_CR_HSION;              // 打开HSI
    while(!(RCC->CR & RCC_CR_HSIRDY));    // 等待HSI就绪

    // 如果不想用PLL，就这样保持 SystemCoreClock=16MHz
    SystemCoreClock = 16000000U;
}

//===== 2) SysTick_Init =====
void SysTick_Init(uint32_t ticks)
{
    SysTick->LOAD  = ticks - 1; 
    SysTick->VAL   = 0; 
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk 
                   | SysTick_CTRL_TICKINT_Msk 
                   | SysTick_CTRL_ENABLE_Msk;
}

//===== GPIO_Init_All =====
void GPIO_Init_All(void)
{
    // 打开 GPIOB (LED) / GPIOC (按键) 时钟
    RCC->AHB1ENR |= (1 << 1); // bit1 => GPIOB
    RCC->AHB1ENR |= (1 << 2); // bit2 => GPIOC

    //===== 配置 PB0, PB1, PB2, PB6, PB7, PB14 => 输出(推挽) =====
    GPIOB->MODER &= ~(
        (3 << (0*2)) | (3 << (1*2)) | (3 << (2*2)) |
        (3 << (6*2)) | (3 << (7*2)) | (3 << (14*2))
    );
    GPIOB->MODER |= (
        (1 << (0*2)) | (1 << (1*2)) | (1 << (2*2)) |
        (1 << (6*2)) | (1 << (7*2)) | (1 << (14*2))
    );

    // 默认全部灭（输出低）
    GPIOB->BSRR = (
        (1 << (0+16)) | (1 << (1+16)) | (1 << (2+16)) |
        (1 << (6+16)) | (1 << (7+16)) | (1 << (14+16))
    );

    //===== 配置 PC13 => 输入 =====
    GPIOC->MODER &= ~(3 << (13*2));  // PC13 -> 输入模式

    // 其他按键(若有)...

    // LED/按键初始化完毕
}

//===== EXTI_Init_All =====
void EXTI_Init_All(void)
{
    // 使能 SYSCFG
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // PC13 => EXTI13 => EXTICR[3], (13-12)=1 => bits[7:4]
    SYSCFG->EXTICR[3] &= ~(0xF << (4*(13-12)));
    SYSCFG->EXTICR[3] |=  (0x2 << (4*(13-12))); // PC=2

    EXTI->IMR  |=  (1 << 13);
    // 按下=1 => 上升沿，松开=0 => 下降沿
    EXTI->RTSR |=  (1 << 13); // 上升沿
    EXTI->FTSR |=  (1 << 13); // 下降沿

    // 使能 EXTI15_10_IRQn
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_SetPriority(EXTI15_10_IRQn, 2);
}

//===== 3) User_Delay =====
void User_Delay(volatile uint32_t ms)
{
    // 简单阻塞
    for(uint32_t i=0; i<ms; i++)
    {
        for(volatile uint32_t j=0; j<2000; j++)
        {
            __NOP();
        }
    }
}