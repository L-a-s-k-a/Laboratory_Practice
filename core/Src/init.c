#include "init.h"

// RCC (Reset and Clock Control) Initialization
void RCC_Ini(void) {
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U); // Adjust internal high-speed clock frequency
    CLEAR_REG(RCC->CFGR); // Reset RCC configuration register
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET); // Wait for reset
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON); // Disable PLL
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET); // Wait for PLL to be disabled
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON); // Disable HSE and CSS
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET); // Wait for HSE to be disabled
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); // Reset HSE bypass

    // Enable external clock (HSE)
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET); // Wait for HSE ready
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); // Reset bypass bit
    SET_BIT(RCC->CR, RCC_CR_CSSON); // Enable clock security system

    // Configure PLL for 180 MHz SYSCLK
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE); // Set PLL source to HSE
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); // Set PLLM to 4 (divide input frequency)
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 |
        RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7); // Set PLLN for 180 MHz
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); // Set PLLP to 2 (divide VCO output frequency)
    SET_BIT(RCC->CR, RCC_CR_PLLON); // Enable PLL
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY)); // Wait for PLL ready

    // Configure AHB and APB prescalers
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); // Set SYSCLK source to PLL
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); // Set AHB prescaler to 1
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4); // Set APB1 prescaler to 4
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2); // Set APB2 prescaler to 2

    // Set flash memory latency for high-speed operation
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}

// GPIO Initialization
void GPIO_Ini(void) {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); // Enable GPIOB and GPIOC
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE2_0 | GPIO_MODER_MODE6_0 |
        GPIO_MODER_MODE1_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE7_0 |
        GPIO_MODER_MODE0_0); // Configure GPIOB pins as output
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_2 | GPIO_OTYPER_OT_6 |
        GPIO_OTYPER_OT_1 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_0); // Set GPIOB pins to push-pull
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR2_0 | GPIO_OSPEEDER_OSPEEDR6_0 |
        GPIO_OSPEEDER_OSPEEDR1_0 | GPIO_OSPEEDER_OSPEEDR14_0 |
        GPIO_OSPEEDER_OSPEEDR7_0 | GPIO_OSPEEDER_OSPEEDR0_0); // Set GPIOB speed
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR6_0 |
        GPIO_PUPDR_PUPDR1_0 | GPIO_PUPDR_PUPDR14_0 |
        GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR0_0); // Disable pull-up/pull-down
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD13_1 | GPIO_PUPDR_PUPD6_1); // Set GPIOC pull-down for PC13 and PC6
}

// EXTI (External Interrupt) Initialization
void EXTI_ITR_Ini(void) {
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); // Enable SYSCFG clock
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, SYSCFG_EXTICR4_EXTI13_PC); // Map EXTI13 to PC13
    SET_BIT(EXTI->IMR, EXTI_IMR_MR13); // Enable interrupt for EXTI13
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13); // Enable rising edge trigger
    CLEAR_BIT(EXTI->FTSR, EXTI_FTSR_TR13); // Disable falling edge trigger
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 2)); // Set EXTI priority
    NVIC_EnableIRQ(EXTI15_10_IRQn); // Enable EXTI IRQ
}

// SysTick Timer Initialization
void SysTick_Init(void) {
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); // Disable SysTick
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk); // Enable SysTick interrupt
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk); // Set SysTick clock source to AHB
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 179999 << SysTick_LOAD_RELOAD_Pos); // Set reload value for 1 ms
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 179999 << SysTick_VAL_CURRENT_Pos); // Clear current value
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); // Enable SysTick
}
