#include "init.h"

void GPIO_Init(void)
{
    // 1. 使能 GPIOB、GPIOC 时钟
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOB_EN | RCC_AHB1ENR_GPIOC_EN);
    
    // 2. 配置 GPIOB 的 PB0/1/2/6/7/14 为通用输出模式 (MODER=01)
    //    先把相应位清零，然后再置1。
    //    每个引脚占 2 bit，比如 PB0 对应 MODER0[1:0], PB1 对应MODER1[1:0], 以此类推。
    
    // 清零 PB0/1/2/6/7/14
    GPIOB->MODER &= ~(
        (3 << (LED_GREEN_PIN * 2)) |
        (3 << (LED_MODE1_PIN  * 2)) |
        (3 << (LED_MODE2_PIN  * 2)) |
        (3 << (LED_MODE3_PIN  * 2)) |
        (3 << (LED_BLUE_PIN   * 2)) |
        (3 << (LED_RED_PIN    * 2))
    );
    // 置为 01 (通用输出)
    GPIOB->MODER |= (
        (1 << (LED_GREEN_PIN * 2)) |
        (1 << (LED_MODE1_PIN  * 2)) |
        (1 << (LED_MODE2_PIN  * 2)) |
        (1 << (LED_MODE3_PIN  * 2)) |
        (1 << (LED_BLUE_PIN   * 2)) |
        (1 << (LED_RED_PIN    * 2))
    );
    
    // 输出模式一般使用 Push-Pull(OTYPER=0), 不需要额外设置时可默认0
    GPIOB->OTYPER &= ~(
        (1 << LED_GREEN_PIN) |
        (1 << LED_MODE1_PIN) |
        (1 << LED_MODE2_PIN) |
        (1 << LED_MODE3_PIN) |
        (1 << LED_BLUE_PIN)  |
        (1 << LED_RED_PIN)
    );
    
    // 速度可设置为中速或低速，这里示例设为中速(01)
    // 以 PB0 为例，对应 OSPEEDR0[1:0] = 01
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
    
    // 无需上下拉
    GPIOB->PUPDR &= ~(
        (3 << (LED_GREEN_PIN * 2)) |
        (3 << (LED_MODE1_PIN  * 2)) |
        (3 << (LED_MODE2_PIN  * 2)) |
        (3 << (LED_MODE3_PIN  * 2)) |
        (3 << (LED_BLUE_PIN   * 2)) |
        (3 << (LED_RED_PIN    * 2))
    );
    
    // 3. 配置 GPIOC 的 PC13/8/9/10 为输入模式 (MODER=00)
    GPIOC->MODER &= ~(
        (3 << (BTN_MODE_PIN * 2)) |
        (3 << (BTN1_PIN * 2)) |
        (3 << (BTN2_PIN * 2)) |
        (3 << (BTN3_PIN * 2))
    );
    
    // 如果按键需要上拉/下拉，则在 PUPDR 设置
    // 例如，如果外部线路是“按下接地”，则可以内部上拉：
    GPIOC->PUPDR &= ~(
        (3 << (BTN_MODE_PIN * 2)) |
        (3 << (BTN1_PIN * 2)) |
        (3 << (BTN2_PIN * 2)) |
        (3 << (BTN3_PIN * 2))
    );
    // 如果要内部上拉:
    // GPIOC->PUPDR |= (
    //    (1 << (BTN_MODE_PIN * 2)) | 
    //    (1 << (BTN1_PIN * 2)) |
    //    (1 << (BTN2_PIN * 2)) |
    //    (1 << (BTN3_PIN * 2))
    // );
    
    // 初始时，先将所有LED都熄灭
    // 可以通过 BSRR 的复位段（高16位）来清零
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
    for(; ms>0; ms--){
        // 这段循环的具体常数需根据编译器和主频微调
        // 只为示意，不做精准延时
        for(volatile uint32_t i=0; i<18000; i++);
    }
}