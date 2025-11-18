#include "Init.h"

#include "stm32f4xx.h"

/* --------------- Тактировние от внешнего кварцевого резонатора 8МГц --------------- */
void Clock_Init_HSE_PLL_168MHz(void)
{
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    SET_BIT(PWR->CR, PWR_CR_VOS);

    /* Enable HSE */
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == 0U) { }

    /* FLASH latency */
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);

    /* Prescalers: AHB = /1, APB1 = /4, APB2 = /2 */
    MODIFY_REG(RCC->CFGR,
               RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2,
               RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2);

    /* PLLM=8, PLLN=336, PLLP=2, PLLQ=7, PLL src = HSE */
    WRITE_REG(RCC->PLLCFGR,
              (8U << RCC_PLLCFGR_PLLM_Pos) |
              (336U << RCC_PLLCFGR_PLLN_Pos) |
              (0U << RCC_PLLCFGR_PLLP_Pos) |
              RCC_PLLCFGR_PLLSRC_HSE |
              (7U << RCC_PLLCFGR_PLLQ_Pos));

    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == 0U) { }

    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) { }

    /* Turn off HSI optionally */
    CLEAR_BIT(RCC->CR, RCC_CR_HSION);

    SystemCoreClock = 168000000U;
}

/* --------------- Системный таймер SysTick --------------- */
void SysTick_Init_1ms(void)
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
    uint32_t reload = 168000000U / 1000U - 1U;
    WRITE_REG(SysTick->LOAD, reload);
    WRITE_REG(SysTick->VAL, 0U);

    CLEAR_BIT(SysTick->CTRL,
              SysTick_CTRL_CLKSOURCE_Msk |
              SysTick_CTRL_TICKINT_Msk |
              SysTick_CTRL_ENABLE_Msk);

    SET_BIT(SysTick->CTRL,
            SysTick_CTRL_CLKSOURCE_Msk |
            SysTick_CTRL_TICKINT_Msk |
            SysTick_CTRL_ENABLE_Msk);
}



/* --------------- Инициализация кнопок и внешних прерываний EXTI --------------- */
void Buttons_GPIO_Init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE0_Msk);
    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE5_Msk);

    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT0_Msk);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT5_Msk);

    /* pull-up */
    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD0_0);
    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD5_0);
}

void ITR_Init(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    // PC13 -> EXTI13
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |= (2<<4); // 2=PortC, биты 7:4 для EXTI13

    EXTI->IMR |= EXTI_IMR_MR13;
    EXTI->RTSR &= ~EXTI_RTSR_TR13;
    EXTI->FTSR |= EXTI_FTSR_TR13;

    NVIC_SetPriority(EXTI15_10_IRQn,2);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void Buttons_EXTI_Init(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    CLEAR_BIT(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0);
    CLEAR_BIT(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI5);

    CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_TR0 | EXTI_RTSR_TR5);
    CLEAR_BIT(EXTI->FTSR, EXTI_FTSR_TR0 | EXTI_FTSR_TR5);

    /* PA0 */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR0);

    /* PA5 */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR5);
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR5);

    SET_BIT(EXTI->IMR, EXTI_IMR_MR0 | EXTI_IMR_MR5);

    WRITE_REG(EXTI->PR, EXTI_PR_PR0 | EXTI_PR_PR5);

    NVIC_SetPriority(EXTI0_IRQn, 5);
    NVIC_EnableIRQ(EXTI0_IRQn);

    NVIC_SetPriority(EXTI9_5_IRQn, 6);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/* --------------- Инициализация диодов --------------- */
void LEDs_GPIO_Init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);

    /* PD1 */
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE1_0);
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT1_Msk);
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR1_1);
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD1_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR1); 

    /* PD2 */
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE2_0);
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT2_Msk);
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR2_1);
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD2_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);

    /* PD3 */
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE3_0);
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT3_Msk);
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR3_1);
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD3_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR3);

    /* PD4 */
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE4_0);
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT4_Msk);
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR4_1);
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD4_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR4); 

    /* PD5 */
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE5_0);
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT5_Msk);
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR5_1);
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD5_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR5);

    /* PD6 */
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE6_0);
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT6_Msk);
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR6_1);
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD6_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR6);
}

void MCO_init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE9_Msk);
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE9_1);

    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT9_Msk);
    CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD9_Msk);

    CLEAR_BIT(GPIOC->AFR[1], 0xFU << ((9U - 8U) * 4U));

    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2 | RCC_CFGR_MCO2PRE);
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2_0 | RCC_CFGR_MCO2_1);
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_0 | RCC_CFGR_MCO2PRE_1 | RCC_CFGR_MCO2PRE_2);
}

void LED_AllOff(void)
{
    GPIOD->BSRR =
        GPIO_BSRR_BR1 | GPIO_BSRR_BR2 | GPIO_BSRR_BR3 |
        GPIO_BSRR_BR4 | GPIO_BSRR_BR5 | GPIO_BSRR_BR6;

    led1_state = 0;
    led2_state = 0;
    led3_state = 0;
    led4_state = 0;
    led5_state = 0;
    led6_state = 0;
}


void LED_On_Index(uint8_t index)
{
    switch (index)
    {
    case 0: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS1); led1_state = 1; break;
    case 1: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2); led2_state = 1; break;
    case 2: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS3); led3_state = 1; break;
    case 3: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS4); led4_state = 1; break;
    case 4: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS5); led5_state = 1; break;
    case 5: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6); led6_state = 1; break;
    }
}

void LED_Off_Index(uint8_t index)
{
    switch (index)
    {
    case 0: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR1); led1_state = 0; break;
    case 1: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2); led2_state = 0; break;
    case 2: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR3); led3_state = 0; break;
    case 3: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR4); led4_state = 0; break;
    case 4: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR5); led5_state = 0; break;
    case 5: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR6); led6_state = 0; break;
    }
}

void LED_SetActive(uint8_t index)
{
    LED_AllOff();
    LED_On_Index(index);
}
