#include "init.h"

/* Частоты для таймера (ARR): 1, 5, 10, 20 Гц.
   PCLK1=42 МГц, TIMCLK=84 МГц, PSC=8399 -> 10 кГц.
   f = 10 000 / (ARR+1) */
static const uint16_t g_arrValues[4] = {
    9999, // 1 Гц
    1999, // 5 Гц
    999,  // 10 Гц
    499   // 20 Гц
};

/* ===== Светодиоды PF7, PF8, PF9, PG1 ===== */

void LEDs_AllOff(void)
{
    /* Сброс PF7, PF8, PF9 */
    WRITE_REG(GPIOF->BSRR, GPIO_BSRR_BR7 | GPIO_BSRR_BR8 | GPIO_BSRR_BR9);
    /* Сброс PG1 */
    WRITE_REG(GPIOG->BSRR, GPIO_BSRR_BR1);
}

void LED_OnIndex(uint8_t index)
{
    switch (index)
    {
    case 0:
        WRITE_REG(GPIOF->BSRR, GPIO_BSRR_BS7);
        break;
    case 1:
        WRITE_REG(GPIOF->BSRR, GPIO_BSRR_BS8);
        break;
    case 2:
        WRITE_REG(GPIOF->BSRR, GPIO_BSRR_BS9);
        break;
    case 3:
        WRITE_REG(GPIOG->BSRR, GPIO_BSRR_BS1);
        break;
    default:
        break;
    }
}

/* ===== GPIO + EXTI0 ===== */

void GPIO_EXTI_Init(void)
{
    /* Тактирование GPIOA, GPIOF, GPIOG и SYSCFG */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN);
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    /* ==== LED: PF7, PF8, PF9 как выходы ==== */
    CLEAR_BIT(GPIOF->MODER,
              GPIO_MODER_MODER7 |
                  GPIO_MODER_MODER8 |
                  GPIO_MODER_MODER9);
    SET_BIT(GPIOF->MODER,
            GPIO_MODER_MODER7_0 |
                GPIO_MODER_MODER8_0 |
                GPIO_MODER_MODER9_0); // 01: output

    CLEAR_BIT(GPIOF->OTYPER, GPIO_OTYPER_OT7 | GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);
    CLEAR_BIT(GPIOF->PUPDR,
              GPIO_PUPDR_PUPDR7 |
                  GPIO_PUPDR_PUPDR8 |
                  GPIO_PUPDR_PUPDR9);

    /* ==== LED: PG1 как выход ==== */
    CLEAR_BIT(GPIOG->MODER, GPIO_MODER_MODER1);
    SET_BIT(GPIOG->MODER, GPIO_MODER_MODER1_0);

    CLEAR_BIT(GPIOG->OTYPER, GPIO_OTYPER_OT1);
    CLEAR_BIT(GPIOG->PUPDR, GPIO_PUPDR_PUPDR1);

    /* ==== Кнопка: PA0 вход + pull-up ==== */
    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODER0); // input
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR0);
    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR0_0); // 01: pull-up

    /* ==== EXTI0 на PA0 ==== */
    /* Порт A в EXTICR1 для линии 0 */
    CLEAR_BIT(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0); // 0000: PA0

    /* Разрешаем линию 0 в маске */
    SET_BIT(EXTI->IMR, EXTI_IMR_MR0);

    /* Триггер по спадающему фронту (нажатие -> 0) */
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR0);
    CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_TR0);

    /* Сброс флага на всякий случай */
    WRITE_REG(EXTI->PR, EXTI_PR_PR0);

    /* NVIC для EXTI0 */
    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

/* ===== TIM2 ===== */

void TIM2_Init(void)
{
    /* Тактируем TIM2 */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);

    /* Останавливаем на время настройки */
    WRITE_REG(TIM2->CR1, 0);
    WRITE_REG(TIM2->CNT, 0);

    /* PSC = 8399 -> 10 кГц */
    WRITE_REG(TIM2->PSC, 8399U);

    /* ARR для 1 Гц (будет использоваться, когда запустим таймер) */
    WRITE_REG(TIM2->ARR, g_arrValues[0]);

    /* Разрешить прерывание по обновлению */
    SET_BIT(TIM2->DIER, TIM_DIER_UIE);

    /* NVIC для TIM2 */
    NVIC_SetPriority(TIM2_IRQn, 2);
    NVIC_EnableIRQ(TIM2_IRQn);

    /* ВНИМАНИЕ: TIM2 пока НЕ запускаем (CEN = 0),
       запуск будет по первому подтверждённому нажатию кнопки. */
}

/* Смена частоты таймера (ARR) */
void TIM2_UpdateFrequency(uint8_t index)
{
    if (index > 3)
        index = 0;

    WRITE_REG(TIM2->ARR, g_arrValues[index]);
    /* Генерируем событие обновления, чтобы новый ARR сразу применился */
    SET_BIT(TIM2->EGR, TIM_EGR_UG);
}

/* ===== SysTick 1 мс ===== */

void SysTick_Init_1ms(void)
{
    /* SysTick от ядра, период 1 мс */
    uint32_t reload = SystemCoreClock / 1000U - 1U;
    WRITE_REG(SysTick->LOAD, reload);
    WRITE_REG(SysTick->VAL, 0U);
    WRITE_REG(SysTick->CTRL,
              SysTick_CTRL_CLKSOURCE_Msk |
                  SysTick_CTRL_TICKINT_Msk |
                  SysTick_CTRL_ENABLE_Msk);
}

/* ===== Clock Init (как в предыдущих лабаъ) ===== */

void Clock_Init_HSE_PLL_168MHz(void)
{
    /* 1. PWR + Scale 1 */
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    SET_BIT(PWR->CR, PWR_CR_VOS);

    /* 2. HSE ON */
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while ((RCC->CR & RCC_CR_HSERDY) == 0U)
    {
    }

    /* 3. FLASH: 5 WS */
    MODIFY_REG(FLASH->ACR,
               FLASH_ACR_LATENCY,
               FLASH_ACR_LATENCY_5WS);

    /* 4. Делители шин */
    MODIFY_REG(RCC->CFGR,
               RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2,
               RCC_CFGR_HPRE_DIV1 |
                   RCC_CFGR_PPRE1_DIV4 |
                   RCC_CFGR_PPRE2_DIV2);

    /* 5. PLL: 8 / 8 * 336 / 2 = 168 МГц */
    WRITE_REG(RCC->PLLCFGR,
              (8U << RCC_PLLCFGR_PLLM_Pos) |
                  (336U << RCC_PLLCFGR_PLLN_Pos) |
                  (0U << RCC_PLLCFGR_PLLP_Pos) |
                  RCC_PLLCFGR_PLLSRC_HSE |
                  (7U << RCC_PLLCFGR_PLLQ_Pos));

    /* 6. PLL ON */
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while ((RCC->CR & RCC_CR_PLLRDY) == 0U)
    {
    }

    /* 7. SYSCLK = PLL */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
    }

    /* 8. HSI OFF */
    CLEAR_BIT(RCC->CR, RCC_CR_HSION);

    /* 9. SystemCoreClock */
    SystemCoreClock = 168000000U;
}
