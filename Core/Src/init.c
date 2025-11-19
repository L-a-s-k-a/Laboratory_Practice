#include "init.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h"

void GPIO_Ini_Self_Def(void)
{
    // 使能GPIOB和GPIOC时钟
    SET_BIT_SELF(RCC_AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    
    // 配置PB7为输出（蓝色LED）
    SET_BIT_SELF(GPIOB_MODER, GPIO_PIN_7_OUT);
    SET_BIT_SELF(GPIOB_OSPEEDR, GPIO_PIN_7_MED);
    SET_BIT_SELF(GPIOB_BSRR, GPIO_PIN_7_RESET);
    
    // PC6按键配置 - 内部上拉
    GPIOC->MODER &= ~GPIO_MODER_MODER6;      // 输入模式
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD6;       // 清除设置
    GPIOC->PUPDR |= GPIO_PUPDR_PUPD6_0;      // 启用内部上拉
    
    // PC13按键配置 - 外部下拉（浮空输入）
    GPIOC->MODER &= ~GPIO_MODER_MODER13;     // 输入模式
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;      // 00: 浮空输入（外部下拉）
}

void GPIO_Ini_CMSIS(void)
{
    /* 使用GPIOB点亮PB14红色LED和PB0绿色LED */
    
    // 配置PB14（红色LED）
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);        // 输出模式
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT14);        // 推挽输出
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0); // 中速
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);              // 初始关闭
    
    // 配置PB0（绿色LED）
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);         // 输出模式
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT0);         // 推挽输出
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0); // 中速
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);             // 初始关闭
}

