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
    //    - 我们用到 GPIOB (6个LED), GPIOC (3个按键)
    RCC->AHB1ENR |= (1 << 1); // Enable GPIOB
    RCC->AHB1ENR |= (1 << 2); // Enable GPIOC

    //---------- 配置 6 个 LED 引脚 (PB0, PB1, PB2, PB6, PB7, PB14) 为输出 ----------
    // 每个引脚 2 位 (MODER寄存器)
    // 00: Input, 01: Output, 10: AF, 11: Analog
    // 这里直接用 "位操作" 或 "MODIFY_REG" 宏均可

    // PB0 => bit 0*2 = 0..1
    // PB1 => bit 1*2 = 2..3
    // PB2 => bit 2*2 = 4..5
    // PB6 => bit 6*2 = 12..13
    // PB7 => bit 7*2 = 14..15
    // PB14 => bit 14*2 = 28..29
    // 先清零，再置对应位为 01 (Output)
    
    // 清零
    GPIOB->MODER &= ~(
        (3 << (0*2))  | (3 << (1*2))  | (3 << (2*2))  |
        (3 << (6*2))  | (3 << (7*2))  | (3 << (14*2))
    );
    // 置 01
    GPIOB->MODER |= (
        (1 << (0*2))  | (1 << (1*2))  | (1 << (2*2))  |
        (1 << (6*2))  | (1 << (7*2))  | (1 << (14*2))
    );

    // 默认先关闭所有 LED (取决于你的硬件，假设输出=0是灭)
    GPIOB->BSRR = (
        (1 << (0+16)) | (1 << (1+16)) | (1 << (2+16)) |
        (1 << (6+16)) | (1 << (7+16)) | (1 << (14+16))
    );

    //---------- 配置 3 个按键 (PC13, PC6, PC7) 为输入(下拉) ----------
    // 先清零
    GPIOC->MODER &= ~(
        (3 << (13*2)) | (3 << (6*2)) | (3 << (7*2))
    );
    // 不设置为上拉/下拉模式时，需要靠外部电路下拉。
    // 如果需要软件下拉，可写:
    //   GPIOC->PUPDR &= ~(...);
    //   GPIOC->PUPDR |= (1 << (13*2)) ... (下拉)
    // 这里假设你板子硬件已做好了下拉，所以可不额外配置PUPDR

    // 好，GPIO 就绪
}

//===== 3) EXTI 初始化 =====
// PC13 => EXTI13, PC6 => EXTI6, PC7 => EXTI7
// 因为 PC6、PC7 都在 EXTI9_5 中断向量里，PC13 在 EXTI15_10
void EXTI_Init_All(void)
{
    // 1. 使能 SYSCFG 时钟 (EXTI 配置需要)
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    //--------- PC13 => EXTI13 ----------
    SYSCFG->EXTICR[3] &= ~(0xF << (4*(13-12))); //清零
    SYSCFG->EXTICR[3] |=  (0x2 << (4*(13-12))); // PC = 2, 写到EXTICR[3]对应bits
    // 使能中断, 下降沿/上升沿 (你说按下是高电平，可根据需要选择上升沿)
    // 如果默认下拉，按下变高，则用上升沿
    EXTI->IMR  |=  (1 << 13);
    EXTI->RTSR |=  (1 << 13);  // 上升沿
    //EXTI->FTSR |=  (1 << 13); // 如果要检测松开，可用下降沿

    // 启用 EXTI15_10_IRQn
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_SetPriority(EXTI15_10_IRQn, 2);

    //--------- PC6 => EXTI6 ----------
    // PC6 对应 EXTICR[1] 的 4*(6-4) = 8..11 位
    SYSCFG->EXTICR[1] &= ~(0xF << (4*(6-4)));
    SYSCFG->EXTICR[1] |=  (0x2 << (4*(6-4)));  // PC = 2
    EXTI->IMR  |=  (1 << 6);
    EXTI->RTSR |=  (1 << 6);

    //--------- PC7 => EXTI7 ----------
    // PC7 对应 EXTICR[1] 的 4*(7-4) = 12..15 位
    SYSCFG->EXTICR[1] &= ~(0xF << (4*(7-4)));
    SYSCFG->EXTICR[1] |=  (0x2 << (4*(7-4)));  // PC = 2
    EXTI->IMR  |=  (1 << 7);
    EXTI->RTSR |=  (1 << 7);

    // PC6、PC7 都属于 EXTI9_5 的中断向量
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