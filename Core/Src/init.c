#include "../Inc/init.h"

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
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);                                             // Включение тактирования периферии SYSCFG
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, SYSCFG_EXTICR4_EXTI13_PC);      // Настройка мультиплексора на вывод линии прерывания EXTI13 на PC13
    SET_BIT(EXTI->IMR, EXTI_IMR_MR13);                                                       // Настройка маскирования 13 линии
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13);                                                     // Настройка детектирования нарастающего фронта 13 линии
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR13);                                                     // Настройка детектирования спадающего фронта 13 линии
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); // Установка 0 приоритета прерывания для вектора EXTI15_10
    NVIC_EnableIRQ(EXTI15_10_IRQn);                                                          // Включение прерывания по вектору EXTI15_10
}

void GPIO_Ini(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN); // Включение тактирования портов GPIOB и GPIOC

    GPIOB_MODER |= GPIOB_MODE_PIN6_OUT;
    GPIOB_OTYPER |= GPIOB_OTYPE_PIN6_PP;
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN6_MID;
    GPIOB_PUPDR |= GPIOB_PUPDR_PIN6_NOPUPD;

    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x004000000UL; // Настройка работы 13-го пина GPIOB в режиме вывода сигнала (Output mode)
    *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x00;          // Настройка на Push-Pull работу 13-го пина GPIOB (Output Push-Pull)
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x001000000UL; // Настройка скорости работы 13-го пина GPIOB на среднюю
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x00;          // Отключение PU/PD резисторов для 13-го пина GPIOB

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE12_0);         // Настройка работы 12-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_12);        // Настройка на Push-Pull работу 12-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR12_0); // Настройка скорости работы 12-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD12_0);       // Отключение PU/PD резисторов для 12-го пина GPIOB

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE15_0);         // Настройка работы 15-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_15);        // Настройка на Push-Pull работу 15-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR15_0); // Настройка скорости работы 15-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD15_0);
}