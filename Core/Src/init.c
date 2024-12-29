#include "init.h"

void GPIO_Init(void)
{
    // 1. 使能 GPIOB、GPIOC 时钟
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOB_EN | RCC_AHB1ENR_GPIOC_EN);
    
    /* 2. 配置 GPIOB 的 PB0/1/2/6/7/14 为通用输出模式 (MODER=01)*/
    GPIOB->MODER &= ~(
        (3 << (LED_GREEN_PIN * 2)) |
        (3 << (LED_MODE1_PIN  * 2)) |
        (3 << (LED_MODE2_PIN  * 2)) |
        (3 << (LED_MODE3_PIN  * 2)) |
        (3 << (LED_BLUE_PIN   * 2)) |
        (3 << (LED_RED_PIN    * 2))
    );
    GPIOB->MODER |= (
        (1 << (LED_GREEN_PIN * 2)) |
        (1 << (LED_MODE1_PIN  * 2)) |
        (1 << (LED_MODE2_PIN  * 2)) |
        (1 << (LED_MODE3_PIN  * 2)) |
        (1 << (LED_BLUE_PIN   * 2)) |
        (1 << (LED_RED_PIN    * 2))
    );
    
    // 推挽输出(OTYPER=0)，速度中速(01)
    GPIOB->OTYPER &= ~(
        (1 << LED_GREEN_PIN) |
        (1 << LED_MODE1_PIN) |
        (1 << LED_MODE2_PIN) |
        (1 << LED_MODE3_PIN) |
        (1 << LED_BLUE_PIN)  |
        (1 << LED_RED_PIN)
    );
    GPIOB->OSPEEDR &= ~(
        (3 << (LED_GREEN_PIN * 2)) |
        (3 << (LED_MODE1_PIN  * 2)) |
        (3 << (LED_MODE2_PIN  * 2)) |
        (3 << (LED_MODE3_PIN  * 2)) |
        (3 << (LED_BLUE_PIN   * 2)) |
        (3 << (LED_RED_PIN    * 2))
    );
    GPIOB->OSPEEDR |= (
        (1 << (LED_GREEN_PIN * 2)) |
        (1 << (LED_MODE1_PIN  * 2)) |
        (1 << (LED_MODE2_PIN  * 2)) |
        (1 << (LED_MODE3_PIN  * 2)) |
        (1 << (LED_BLUE_PIN   * 2)) |
        (1 << (LED_RED_PIN    * 2))
    );
    GPIOB->PUPDR &= ~(
        (3 << (LED_GREEN_PIN * 2)) |
        (3 << (LED_MODE1_PIN  * 2)) |
        (3 << (LED_MODE2_PIN  * 2)) |
        (3 << (LED_MODE3_PIN  * 2)) |
        (3 << (LED_BLUE_PIN   * 2)) |
        (3 << (LED_RED_PIN    * 2))
    );
    
    /* 3. 配置 GPIOC 的 PC12/8/9/10 为输入模式 (MODER=00)*/
    GPIOC->MODER &= ~(
        (3 << (BTN_MODE_PIN * 2)) |  // PC12
        (3 << (BTN1_PIN * 2))     |  // PC8
        (3 << (BTN2_PIN * 2))     |  // PC9
        (3 << (BTN3_PIN * 2))        // PC10
    );
    
    // 为 PC12, PC8, PC9, PC10 启用内部上拉 (PUPDR=01)
    // （按下接地 -> 0电平；松开时由上拉-> 1电平）
    GPIOC->PUPDR &= ~(
        (3 << (BTN_MODE_PIN * 2)) |
        (3 << (BTN1_PIN * 2))     |
        (3 << (BTN2_PIN * 2))     |
        (3 << (BTN3_PIN * 2))
    );
    GPIOC->PUPDR |= (
        (1 << (BTN_MODE_PIN * 2)) |  // PC12 -> 上拉
        (1 << (BTN1_PIN * 2))     |  // PC8  -> 上拉
        (1 << (BTN2_PIN * 2))     |  // PC9  -> 上拉
        (1 << (BTN3_PIN * 2))        // PC10 -> 上拉
    );
    
    // 4. 初始时，先将所有LED都熄灭
    GPIOB->BSRR = (
        (1 << (LED_GREEN_PIN + 16)) |
        (1 << (LED_BLUE_PIN  + 16)) |
        (1 << (LED_RED_PIN   + 16)) |
        (1 << (LED_MODE1_PIN + 16)) |
        (1 << (LED_MODE2_PIN + 16)) |
        (1 << (LED_MODE3_PIN + 16))
    );
}

/**
 * 简易延时函数(软件方式)，仅作演示
 * 实际中可用SysTick或硬件定时器
 */
void delay_ms(volatile uint32_t ms)
{
    for(; ms>0; ms--)
    {
        for(volatile uint32_t i=0; i<18000; i++);
    }
}