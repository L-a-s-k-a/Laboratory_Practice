#include "init1.h"

/* LED1-LED3: PB0, PB7, PB14; LED4-LED6: PA5, PA6, PA7. */
const Led_t Leds[LED_COUNT] = {
    {GPIOB, 0}, {GPIOB, 7}, {GPIOB, 14},
    {GPIOA, 5}, {GPIOA, 6}, {GPIOA, 7}
};

void Led_On(uint8_t index)
{
    Leds[index].port->BSRR = 1UL << Leds[index].pin;
}

void Led_Off(uint8_t index)
{
    Leds[index].port->BSRR = 1UL << (Leds[index].pin + 16U);
}

void Leds_AllOff(void)
{
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        Led_Off(i);
    }
}

void RCC_Ini(void)
{
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);

    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);

    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);
    SET_BIT(RCC->CR, RCC_CR_CSSON);

    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    MODIFY_REG(PWR->CR, PWR_CR_VOS, PWR_CR_VOS);

    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, 4U << RCC_PLLCFGR_PLLM_Pos);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN, 180U << RCC_PLLCFGR_PLLN_Pos);
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ, 8U << RCC_PLLCFGR_PLLQ_Pos);

    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);

    SET_BIT(PWR->CR, PWR_CR_ODEN);
    while (READ_BIT(PWR->CSR, PWR_CSR_ODRDY) == RESET);
    SET_BIT(PWR->CR, PWR_CR_ODSWEN);
    while (READ_BIT(PWR->CSR, PWR_CSR_ODSWRDY) == RESET);

    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN);
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_5WS);

    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);

    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void GPIO_Ini(void)
{
    SET_BIT(RCC->AHB1ENR,
            RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    (void)RCC->AHB1ENR;

    for (uint8_t i = 0; i < LED_COUNT; i++) {
        GPIO_TypeDef *port = Leds[i].port;
        uint8_t pin = Leds[i].pin;

        MODIFY_REG(port->MODER, 3UL << (pin * 2U), 1UL << (pin * 2U));
        CLEAR_BIT(port->OTYPER, 1UL << pin);
        MODIFY_REG(port->OSPEEDR, 3UL << (pin * 2U), 1UL << (pin * 2U));
        CLEAR_BIT(port->PUPDR, 3UL << (pin * 2U));
    }

    Leds_AllOff();

    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE13);
    MODIFY_REG(GPIOC->PUPDR, GPIO_PUPDR_PUPD13, GPIO_PUPDR_PUPD13_1);
}

void EXTI_ITR_Ini(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
    MODIFY_REG(SYSCFG->EXTICR[3],
               SYSCFG_EXTICR4_EXTI13_Msk,
               SYSCFG_EXTICR4_EXTI13_PC);

    SET_BIT(EXTI->IMR, EXTI_IMR_MR13);
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13);
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR13);

    NVIC_SetPriority(EXTI15_10_IRQn,
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void SysTick_Init(void)
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_CLKSOURCE_Msk);
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 180000UL - 1UL);
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 0UL);

    NVIC_SetPriority(SysTick_IRQn,
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}
