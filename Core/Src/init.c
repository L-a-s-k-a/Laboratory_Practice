

#include "init.h"

//===== 1) System Clock 配置 =====
// 下面是一个典型的 180MHz 配置示例(PLL), 具体数值请结合实际情况或CubeMX生成
void SystemClock_Config(void)
{
    // 这里给出一个简化的示例，实际项目中常用 CubeMX 配置或手动完善
    // 启用HSE / 配置PLL / 切换系统时钟到PLL / 设置总线分频 等等
    // 为示例起见，这里可能只是默认HSI或做最小配置

    // [伪代码/占位] 仅保证能够正常跑
    RCC->CR |= RCC_CR_HSION;                  // 打开HSI
    while ((RCC->CR & RCC_CR_HSIRDY) == 0);   // 等待HSI就绪
    
    // 让 SystemCoreClock = 16MHz (假设)
    // 如果你需要更高主频，请参考参考手册配置PLL
    // ...
    SystemCoreClock = 16000000U; // 用于 SysTick 计算
}

//===== 2) GPIO 初始化 =====
void GPIO_Init_All(void)
{
    // 1. 使能各 GPIO 时钟
    RCC->AHB1ENR |= (1 << 1); // Enable GPIOB (LED)
    RCC->AHB1ENR |= (1 << 2); // Enable GPIOC (BUTTON)

    //---------- 配置 6 个 LED 引脚 (PB0, PB1, PB2, PB6, PB7, PB14) 为输出 ----------
    GPIOB->MODER &= ~(
        (3 << (0*2))  | (3 << (1*2))  | (3 << (2*2))  |
        (3 << (6*2))  | (3 << (7*2))  | (3 << (14*2))
    );
    GPIOB->MODER |= (
        (1 << (0*2))  | (1 << (1*2))  | (1 << (2*2))  |
        (1 << (6*2))  | (1 << (7*2))  | (1 << (14*2))
    );

    // 默认先关闭所有 LED
    GPIOB->BSRR = (
        (1 << (0+16)) | (1 << (1+16)) | (1 << (2+16)) |
        (1 << (6+16)) | (1 << (7+16)) | (1 << (14+16))
    );

    //---------- 配置 3 个按键 (PC13, PC6, PC3) => 下拉输入 ----------
    // 先设置为输入模式 MODER=00
    GPIOC->MODER &= ~(
        (3 << (13*2)) |
        (3 << (6*2))  |
        (3 << (3*2))
    );

    // 再设置 PUPDR=10 => pull-down
    GPIOC->PUPDR &= ~(
        (3 << (13*2)) |
        (3 << (6*2))  |
        (3 << (3*2))
    );
    GPIOC->PUPDR |= (
        (2 << (13*2)) |
        (2 << (6*2))  |
        (2 << (3*2))
    );
}


//===== 3) EXTI 初始化 =====
// PC13 => EXTI13, PC6 => EXTI6, PC7 => EXTI7
// 因为 PC6、PC7 都在 EXTI9_5 中断向量里，PC13 在 EXTI15_10
void EXTI_Init_All(void)
{
    // 1. SYSCFG 时钟
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    //=== 1) PC13 => EXTI13 (模式切换) ===
    SYSCFG->EXTICR[3] &= ~(0xF << (4*(13-12)));
    SYSCFG->EXTICR[3] |=  (0x2 << (4*(13-12))); // PC = 2
    EXTI->IMR  |=  (1 << 13);
    // 下拉 + 按下=1 => 上升沿
    EXTI->RTSR |=  (1 << 13);
    EXTI->FTSR &= ~(1 << 13);

    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_SetPriority(EXTI15_10_IRQn, 2);

    //=== 2) PC6 => EXTI6 (按钮2，调频率) ===
    // EXTI6 在 EXTI9_5 向量中
    SYSCFG->EXTICR[1] &= ~(0xF << (4*(6-4)));
    SYSCFG->EXTICR[1] |=  (0x2 << (4*(6-4))); // PC=2
    EXTI->IMR  |=  (1 << 6);
    EXTI->RTSR |=  (1 << 6);  // 上升沿
    EXTI->FTSR &= ~(1 << 6);

    //=== 3) PC3 => EXTI3 (按钮3，切换 LED 编号) ===
    // EXTI3 在 EXTI3 向量中
    SYSCFG->EXTICR[0] &= ~(0xF << (4*3));
    SYSCFG->EXTICR[0] |=  (0x2 << (4*3)); // PC=2
    EXTI->IMR  |=  (1 << 3);
    EXTI->RTSR |=  (1 << 3); 
    EXTI->FTSR &= ~(1 << 3);

    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_SetPriority(EXTI3_IRQn, 2);

    NVIC_EnableIRQ(EXTI9_5_IRQn);
    NVIC_SetPriority(EXTI9_5_IRQn, 2);
}


//===== 4) SysTick 初始化 =====
void SysTick_Init(uint32_t ticks)
{
    // SysTick 配置，让它每 `ticks` 个 CPU Cycle 触发一次中断
    // 常见是 1ms = SystemCoreClock/1000
    SysTick->LOAD  = ticks - 1;
    SysTick->VAL   = 0; 
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk
                   | SysTick_CTRL_TICKINT_Msk
                   | SysTick_CTRL_ENABLE_Msk;
}

void User_Delay(volatile uint32_t ms)
{
    // 简单 busy-wait
    // 如果 SysTick 频率 1ms 一次，则：
    for(uint32_t i=0; i<ms; i++)
    {
        // 这里可以插入 nop 或判断 SysTick 标志
        for(volatile uint32_t j=0; j<2000; j++) 
        {
            __NOP();
        }
    }
}
