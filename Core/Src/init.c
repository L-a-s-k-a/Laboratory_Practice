#include "init.h"
void GPIO_Ini(void)
{

    GPIOC_MODER |= GPIOC_MODE_OUT_PIN6;
    GPIOC_TYPER |= GPIOC_TYPE_PP_PIN6;
    GPIOC_SPEEDR |= GPIOC_SPEED_M_PIN6;
    GPIOC_PUPDR |= GPIOC_PUPDR_NPUPD_PIN6;

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN);

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE3_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE1_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE4_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);  // Вторая лаба
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);  // Вторая лаба
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0); // Вторая лаба
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE4_1);  // Вторая лаба
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE5_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE13_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE15_0);
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE6_0);
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE7_0);
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE6_0);
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE7_0);

    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_0 | GPIO_OTYPER_OT_1);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0 | GPIO_OSPEEDER_OSPEEDR5_0 | GPIO_OSPEEDER_OSPEEDR13_0 | GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR1_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR5_0 | GPIO_PUPDR_PUPDR13_0 | GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_0 | GPIO_OTYPER_OT_14 | GPIO_OTYPER_OT_4);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0 | GPIO_OSPEEDER_OSPEEDR14_0 | GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR4_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0 | GPIO_PUPDR_PUPDR14_0 | GPIO_PUPDR_PUPDR0_0);
}

void RCC_Init(void)
{ /* Предварительная очистка регистров RCC */
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET)
        ;
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET)
        ;
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET)
        ;
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); /* Настройка главного регистра RCC */
    SET_BIT(RCC->CR, RCC_CR_HSEON);    // Запускаем внешний кварцевый резонатор
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET)
        ;                                      // Ждём пока он запустится
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);         // Сбросим бит байпаса в 0, если вдруг там что-то лежит
    SET_BIT(RCC->CR, RCC_CR_CSSON);            // Запустим Clock detector
                                               /* Настройка регистров PLL
                                                * Предварительная очистка регистра
                                                * В качестве источника тактирования для PLL выбирается HSE
                                                * Мы сначала делим входную частоту (HSE) на 4 (получаем 2 МГц), затем умножаем на 180 и снова делим на 2, таким образом получаем 180МГц
                                                * Включаем работу PLL */
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0); // CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); // Выставляем предделитель входной частоты PLL на 4
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7);
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); // Настраиваем предделитель получившейся частоты после умножения. Иными словами, получаем итоговую частоту PLL
    SET_BIT(RCC->CR, RCC_CR_PLLON);                // Запустим PLL
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY))
        ; // Ждём запуска PLL
    /* Настройка основных конфигураций RCC
     * В качетсве системных часов выбираем выход PLL
     * Настраиваем предделители шин AHB и APB
     * Настраиваем выходы MCO1 и MCO2 для внешней оценки настроенной системы тактирования */
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);           // Выбираем PLL в качестве System Clock
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);      // Предделитель AHB, без делителя
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);    // Предделитель APВ1, делим на 4
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);    // Предделитель APВ2, делим на 2
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk); // Предделитель на выходе MCO2 (PC9) = 5
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);                           // Настраиваем на выход MCO2 - System clock
    /* Настройка задержки внутренней памяти
     * Выставление битов LATENCY регистра FLASH_ACR в позицию 5SW (6 CPU cycles).
     * Данная настройка необходима при увеличении системной частоты тактирования свыше 20 МГц. Таблица 12
     */
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}

void ITR_init(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);                                         // Включение тактирования периферии SYSCFG
    MODIFY_REG(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI4_Msk, SYSCFG_EXTICR2_EXTI4_PB);    // Настройка мультиплексора на вывод линии прерывания EXTI4
    SET_BIT(EXTI->IMR, EXTI_IMR_MR4);                                                    // Настройка маскирования 4
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR4);                                                  // Настройка детектирования нарастающего фронта 4
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR4);                                                  // Настройка детектирования спадающего фронта 4
    NVIC_SetPriority(EXTI4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); // Установка 0 приоритета прерывания для вектора EXTI4
    NVIC_EnableIRQ(EXTI4_IRQn);                                                          // Включение прерывания по вектору EXTI4
}

void SysTick_Init(void)
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                     // На всякий случай, предварительно, выключим счётчик
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);                                      // Разрешаем прерывание по системному таймеру
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);                                    // Источник тактирования будет идти из AHB без деления
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 179999 << SysTick_LOAD_RELOAD_Pos); // Значение с которого начинается счёт, эквивалентное 1 кГц
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 179999 << SysTick_VAL_CURRENT_Pos);  // Очистка поля
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                       // Включим счётчик
}