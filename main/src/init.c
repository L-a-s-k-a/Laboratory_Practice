#include "init.h"
#include "stm32f429xx.h"
#include "stdio.h"

void SystemClock_Config(void)
{
    // 使用默认时钟配置
    RCC->CR |= RCC_CR_HSEON;
    while(!(RCC->CR & RCC_CR_HSERDY));
    
    RCC->CFGR = RCC_CFGR_SW_HSE;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);
    
    SystemCoreClockUpdate();
}

void GPIO_Init(void)
{
    // 启用GPIO时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | 
                    RCC_AHB1ENR_GPIOCEN | 
                    RCC_AHB1ENR_GPIOEEN |
                    RCC_AHB1ENR_GPIOGEN;
    
    // 配置LED引脚为输出
    // 板载LED (PB0, PB7, PB14)
    GPIOB->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER14_0;
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT7 | GPIO_OTYPER_OT14);
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED0_1 | GPIO_OSPEEDR_OSPEED7_1 | GPIO_OSPEEDR_OSPEED14_1;
    
    // 外部LED (PC12, PE2, PE3)
    GPIOC->MODER |= GPIO_MODER_MODER12_0;  // PC12设为输出
    GPIOE->MODER |= GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0;  // PE2, PE3设为输出
    
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT12;   // PC12推挽输出
    GPIOE->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);  // PE2, PE3推挽输出
    
    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEED12_1;  // PC12高速
    GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED2_1 | GPIO_OSPEEDR_OSPEED3_1;  // PE2, PE3高速
    
    // 配置按钮引脚为输入
    // 按钮1 (PG3) - 使用PG3替代损坏的板载按钮
    GPIOG->MODER &= ~GPIO_MODER_MODER3;    // PG3设为输入
    GPIOG->PUPDR |= GPIO_PUPDR_PUPDR3_0;   // PG3上拉
    
    // 外部按钮 (PE0, PE4)
    GPIOE->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER4);  // PE0, PE4设为输入
    GPIOE->PUPDR |= GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR4_0; // PE0, PE4上拉
}

void LED_On(uint8_t led_num)
{
    switch(led_num) {
        case 1: GPIOB->BSRR = LED1_PIN; break;
        case 2: GPIOB->BSRR = LED2_PIN; break;
        case 3: GPIOB->BSRR = LED3_PIN; break;
        case 4: GPIOC->BSRR = LED4_PIN; break;
        case 5: GPIOE->BSRR = LED5_PIN; break;
        case 6: GPIOE->BSRR = LED6_PIN; break;
    }
}

void LED_Off(uint8_t led_num)
{
    switch(led_num) {
        case 1: GPIOB->BSRR = (LED1_PIN << 16); break;
        case 2: GPIOB->BSRR = (LED2_PIN << 16); break;
        case 3: GPIOB->BSRR = (LED3_PIN << 16); break;
        case 4: GPIOC->BSRR = (LED4_PIN << 16); break;
        case 5: GPIOE->BSRR = (LED5_PIN << 16); break;
        case 6: GPIOE->BSRR = (LED6_PIN << 16); break;
    }
}

uint8_t Button_Read(uint8_t btn_num)
{
    switch(btn_num) {
        case 1: 
            // 读取PG3，按下为低电平
            return (GPIOG->IDR & BTN1_PIN) == 0;
        case 2: 
            // 读取PE0，按下为低电平
            return (GPIOE->IDR & BTN2_PIN) == 0;
        case 3: 
            // 读取PE4，按下为低电平
            return (GPIOE->IDR & BTN3_PIN) == 0;
        default: 
            return 0;
    }
}

void All_LEDs_Off(void)
{
    for(uint8_t i = 1; i <= 6; i++) {
        LED_Off(i);
    }
}