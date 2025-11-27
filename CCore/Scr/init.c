#include "../Inc/init.h"

uint32_t counter_timer = 0;
uint8_t type = 0;
uint32_t types = 0;
bool k = false;

void GPIO_Init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOBEN);

    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD3_0);
}

void RCC_Init(void)
{
    /*Предварительная очистка регистров RCC */
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80UL); // Очистка битов HSITRIM и установка 1000 0000
    CLEAR_REG(RCC->CFGR);                        // Очистка регистра CFGR
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET)
        ;                             // Проверка очистки регистра CR
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON); // Очистка бита PLLON в регистре CR
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET)
        ;                                            // Проверка очистки бита PLLON
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON); // Очистка битов HSEON, CSSON в регистре CR
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET)
        ;                              // Проверка очистки битов HSEON, CSSON
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); // Очистка бита HSEBYP в регистре CR

    /*Настройка главного регистра RCC */
    SET_BIT(RCC->CR, RCC_CR_HSION); // Запускаем HSI
    while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET)
        ; // Ждём пока он запустится

    /*Настройка регистра PLLCFGR*/
    CLEAR_REG(RCC->PLLCFGR);                                        // Очищаем регистр PLLCFGR
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSI);                  // Источник тактирования HSE
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_3);                      // Деление источника тактирования на 8
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_5); // Настрока умножения на 96
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0);                    // Делитель для выхода PLL(PLLCLK) на 4
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);                      // Делитель для USB на 4

    /*Настройка регистра CFGR*/
    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);                         // Источник системного тактирования -> выход PLL
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV1);                    // Делитель AHB1 -> 0
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV2);                     // Делитель APB1 -> 2 (48 МГц)
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE2_DIV1);                     // Делитель APB2 -> 1 (96 МГц)
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);                         // Выход MCO2 -> Sysclk
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_1); // Делитель для MCO2 -> 4

    /*Настрйока частоты FLASH-памяти*/
    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_3WS);

    /*Включение блока PLL*/
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET)
        ;
}

void TIM10_Init(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM10EN);

    // при 96 МГц тактовой  96 000 000 / (95+1) / (999+1) = 1000 Гц
    MODIFY_REG(TIM10->PSC, TIM_PSC_PSC_Msk, 95UL); // PSC = 2
    MODIFY_REG(TIM10->ARR, TIM_ARR_ARR_Msk, 999UL);     // ARR = 799 → 20 кГц

    // Генерируем Update-событие (чтобы сразу загрузились PSC и ARR)
    SET_BIT(TIM10->EGR, TIM_EGR_UG);

    // Генерируем Update-событие (чтобы сразу загрузились PSC и ARR)
    SET_BIT(TIM10->DIER, TIM_DIER_UIE);

    // Включаем прерывание в NVIC
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    // Запускаем таймер
    SET_BIT(TIM10->CR1, TIM_CR1_CEN);
}

void IQR_Init(void)
{
    /*Включение тактирование периферии SYSCFG*/
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    /*Обозначение снятие внешеного сигнала с линии PC3*/
    SET_BIT(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_PC);
    SET_BIT(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI3_PC);

    /*Настройка регистров EXTI*/
    SET_BIT(EXTI->IMR, EXTI_IMR_IM13 | EXTI_IMR_IM3);    // Включение маскирование
    CLEAR_BIT(EXTI->EMR, EXTI_EMR_EM13 | EXTI_EMR_EM3);  // Отключение генерации события
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13 | EXTI_RTSR_TR3); // Настройка отслеживания по спаду (кнопка была отпущена)
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR13 | EXTI_FTSR_TR3); // Настройка отслеживания по фронту (кнопка была нажата)

    /*Настройка регистров NVIC*/
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); // Выбор приоретета прерывания
    NVIC_EnableIRQ(EXTI15_10_IRQn);                                                          // Разрешение прерывания

    /*Настройка регистров NVIC*/
    NVIC_SetPriority(EXTI3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); // Выбор приоретета прерывания
    NVIC_EnableIRQ(EXTI3_IRQn);                                                          // Разрешение прерывания
}

void TIM3_PWM_Init(void)
{
    // Тактирование порта B и TIM3
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);

    // PB4 и PB5 в Alternate Function mode
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE4 | GPIO_MODER_MODE5, GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1);

    // Назначаем AF2 (TIM3) для PB4 и PB5
    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5, 2UL << GPIO_AFRL_AFSEL4_Pos | 2UL << GPIO_AFRL_AFSEL5_Pos);

    // Останавливаем таймер перед настройкой
    CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);

    // при 48 МГц тактовой  48 000 000 / (2+1) / (799+1) = 20 000 Гц
    MODIFY_REG(TIM3->PSC, TIM_PSC_PSC_Msk, 2UL);   // PSC = 2
    MODIFY_REG(TIM3->ARR, TIM_ARR_ARR_Msk, 799UL); // ARR = 799 → 20 кГц

    // Включаем preload для ARR
    SET_BIT(TIM3->CR1, TIM_CR1_ARPE);

    // Канал 1  PWM mode 1 (110) + preload
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC1PE_Msk, (6UL << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE);

    // Канал 2  PWM mode 1 (110) + preload
    MODIFY_REG(TIM3->CCMR1, TIM_CCMR1_OC2M_Msk | TIM_CCMR1_OC2PE_Msk, (6UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE); // 6 = 110b

    // Включаем выходы каналов
    SET_BIT(TIM3->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);

    // Начальные значения скважности = 0%
    MODIFY_REG(TIM3->CCR1, TIM_CCR1_CCR1_Msk, 0UL);
    MODIFY_REG(TIM3->CCR2, TIM_CCR2_CCR2_Msk, 0UL);

    // Генерируем Update-событие (чтобы сразу загрузились PSC и ARR)
    SET_BIT(TIM3->EGR, TIM_EGR_UG);

    // Запускаем таймер
    SET_BIT(TIM3->CR1, TIM_CR1_CEN);
}

void TIM1_PWM_Init(void)
{
    // Тактирование порта A и TIM1
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

    // PA8 и PA9 в Alternate Function mode
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE8 | GPIO_MODER_MODE9, GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);

    // Назначаем AF2 (TIM3) для PB4 и PB5
    MODIFY_REG(GPIOA->AFR[1], GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9, 1UL << GPIO_AFRH_AFSEL8_Pos | 1UL << GPIO_AFRH_AFSEL9_Pos);

    // Останавливаем таймер перед настройкой
    CLEAR_BIT(TIM1->CR1, TIM_CR1_CEN);

    // при 48 МГц тактовой  48 000 000 / (2+1) / (799+1) = 20 000 Гц
    MODIFY_REG(TIM1->PSC, TIM_PSC_PSC_Msk, 5UL);   // PSC = 2
    MODIFY_REG(TIM1->ARR, TIM_ARR_ARR_Msk, 799UL); // ARR = 799 → 20 кГц

    // Включаем preload для ARR
    SET_BIT(TIM1->CR1, TIM_CR1_ARPE);

    // Канал 1  PWM mode 1 (110) + preload
    MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC1PE_Msk, (6UL << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE);

    // Канал 2  PWM mode 1 (110) + preload
    MODIFY_REG(TIM1->CCMR1, TIM_CCMR1_OC2M_Msk | TIM_CCMR1_OC2PE_Msk, (6UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE); // 6 = 110b

    // Включаем выходы каналов
    SET_BIT(TIM1->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);
    SET_BIT(TIM1->BDTR, TIM_BDTR_MOE);

    // Начальные значения скважности = 0%
    MODIFY_REG(TIM1->CCR1, TIM_CCR1_CCR1_Msk, 0UL);
    MODIFY_REG(TIM1->CCR2, TIM_CCR2_CCR2_Msk, 0UL);

    // Генерируем Update-событие (чтобы сразу загрузились PSC и ARR)
    SET_BIT(TIM1->EGR, TIM_EGR_UG);

    // Запускаем таймер
    SET_BIT(TIM1->CR1, TIM_CR1_CEN);
}

void TIM5_PWM_Init(void)
{
    // Тактирование порта A и TIM3
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM5EN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

    // PA0 и PA1 в Alternate Function mode
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODE0 | GPIO_MODER_MODE1, GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1);

    // Назначаем AF1 (TIM3) для PA0 и PA1
    MODIFY_REG(GPIOA->AFR[0], GPIO_AFRL_AFSEL0 | GPIO_AFRL_AFSEL1, 2UL << GPIO_AFRL_AFSEL0_Pos | 2UL << GPIO_AFRL_AFSEL1_Pos);

    // Останавливаем таймер перед настройкой
    CLEAR_BIT(TIM5->CR1, TIM_CR1_CEN);

    // при 48 МГц тактовой  48 000 000 / (2+1) / (799+1) = 20 000 Гц
    MODIFY_REG(TIM5->PSC, TIM_PSC_PSC_Msk, 2UL);   // PSC = 2
    MODIFY_REG(TIM5->ARR, TIM_ARR_ARR_Msk, 799UL); // ARR = 799 → 20 кГц

    // Включаем preload для ARR
    SET_BIT(TIM5->CR1, TIM_CR1_ARPE);

    // Канал 1  PWM mode 1 (110) + preload
    MODIFY_REG(TIM5->CCMR1, TIM_CCMR1_OC1M_Msk | TIM_CCMR1_OC1PE_Msk, (6UL << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE);

    // Канал 2  PWM mode 1 (110) + preload
    MODIFY_REG(TIM5->CCMR1, TIM_CCMR1_OC2M_Msk | TIM_CCMR1_OC2PE_Msk, (6UL << TIM_CCMR1_OC2M_Pos) | TIM_CCMR1_OC2PE); // 6 = 110b

    // Включаем выходы каналов
    SET_BIT(TIM5->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);

    // Начальные значения скважности = 0%
    MODIFY_REG(TIM5->CCR1, TIM_CCR1_CCR1_Msk, 0UL);
    MODIFY_REG(TIM5->CCR2, TIM_CCR2_CCR2_Msk, 0UL);

    // Генерируем Update-событие (чтобы сразу загрузились PSC и ARR)
    SET_BIT(TIM5->EGR, TIM_EGR_UG);

    // Запускаем таймер
    SET_BIT(TIM5->CR1, TIM_CR1_CEN);
}

void Flickering_LED(uint32_t Frequency)
{
    switch (type)
    {
    case 0:
        types = 0UL;
        break;
    case 1:
        types = 200UL;
        break;
    case 2:
        types = 400UL;
        break;
    case 3:
        types = 600UL;
        break;
    case 4:
        types = 800UL;
        break;
    }
    if (counter_timer >= 0 && counter_timer < Frequency)
    {
        k = false;
        MODIFY_REG(TIM3->CCR1, TIM_CCR1_CCR1_Msk, 0UL);
        MODIFY_REG(TIM3->CCR2, TIM_CCR2_CCR2_Msk, 0UL);
        MODIFY_REG(TIM5->CCR1, TIM_CCR1_CCR1_Msk, 0UL);
        MODIFY_REG(TIM5->CCR2, TIM_CCR2_CCR2_Msk, 0UL);
        MODIFY_REG(TIM1->CCR2, TIM_CCR1_CCR1_Msk, 0UL);
        MODIFY_REG(TIM1->CCR1, TIM_CCR1_CCR1_Msk, types);
    }
    else if (counter_timer >= Frequency && counter_timer < 2 * Frequency)
    {
        k = true;
        MODIFY_REG(TIM1->CCR1, TIM_CCR1_CCR1_Msk, 0UL);
        MODIFY_REG(TIM1->CCR2, TIM_CCR2_CCR2_Msk, types);
    }
    else if (counter_timer >= 2 * Frequency && counter_timer < 3 * Frequency)
    {
        k = false;
        MODIFY_REG(TIM1->CCR2, TIM_CCR1_CCR1_Msk, 0UL);
        MODIFY_REG(TIM3->CCR1, TIM_CCR2_CCR2_Msk, types);
    }
    else if (counter_timer >= 3 * Frequency && counter_timer < 4 * Frequency)
    {
        MODIFY_REG(TIM3->CCR1, TIM_CCR1_CCR1_Msk, 0UL);
        MODIFY_REG(TIM3->CCR2, TIM_CCR2_CCR2_Msk, types);
    }
    else if (counter_timer >= 4 * Frequency && counter_timer < 5 * Frequency)
    {
        MODIFY_REG(TIM3->CCR2, TIM_CCR1_CCR1_Msk, 0UL);
        MODIFY_REG(TIM5->CCR1, TIM_CCR2_CCR2_Msk, types);
    }
    else if (counter_timer >= 5 * Frequency && counter_timer < 6 * Frequency)
    {
        MODIFY_REG(TIM5->CCR1, TIM_CCR1_CCR1_Msk, 0UL);
        MODIFY_REG(TIM5->CCR2, TIM_CCR2_CCR2_Msk, types);
    }
    else if (counter_timer >= 6 * Frequency)
    {
        counter_timer = 0;
    }
}
