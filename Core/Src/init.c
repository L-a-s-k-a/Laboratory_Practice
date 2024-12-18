#include "init.h"

void RCC_Ini(void)
{
        /* Предварительная очистка регистров RCC */
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
        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
        /* Настройка главного регистра RCC */
        SET_BIT(RCC->CR, RCC_CR_HSEON); // Запускаем внешний кварцевый резонатор
        while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET)
                ;                          // Ждём пока он запустится
        CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); // Сбросим бит байпаса в 0, если вдруг там что-то лежит
        SET_BIT(RCC->CR, RCC_CR_CSSON);    // Запустим Clock detector
        /* Натсройка регистров PLL
        * Предварительная очистка регистра
        * В качестве источника тактирования для PLL выбирается HSE
        * Мы сначала делим входную частоту (HSE) на 4 (получаем 2 МГц),
        затем умножем на 180 и снова делим на 2, таким образом получем 180МГц
        * Включаем работу PLL
        */
        CLEAR_REG(RCC->PLLCFGR);
        SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
        MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2);
        // Выставляем предделитель входной частоты PLL на 4
        MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7);
        CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); // Настраиваем предделитель получившейся частоты после умножения. Иными словами, получаем итоговую частоту PLL
        SET_BIT(RCC->CR, RCC_CR_PLLON);                // Запустим PLL
        while (READ_BIT(RCC->CR, RCC_CR_PLLRDY))
                ; // Ждём запуска PLL

        /* Настройка основных конфигураций RCC
        * В качетсве системных часов выбираем выход PLL
        * Настраиваем предделители шин AHB и APB
        * Настраиваем выходы MCO1 и MCO2 для внешней оценки настроенной
        системы тактирования
        */
        MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); // Выбираем PLL в качестве System Clock
        MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);
        // Предделитель AHB, без делителя
        MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);
        // Предделитель APВ1, делим на 4
        MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);
        // Предделитель APВ2, делим на 2
        MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk);
        // Предделитель на выходе MCO2 (PC9) = 5
        CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2); // Настраиваем на выход MCO2 - System clock
        /* Настройка задержки внутренней памяти
        * Выставление битов LATENCY регистра FLASH_ACR в позицию 5SW (6 CPU
        cycles).
        * Данная настройка необходима при увеличении системной частоты
        тактирования свыше 20 МГц. Таблица 12 RM0090
        */
        MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}
void GPIO_Ini(void)
{
        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); // Включение тактирования портов GPIOB и GPIOC

        SET_BIT(GPIOB->MODER, GPIO_MODER_MODER8_0);
        SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);

        SET_BIT(GPIOB->MODER, GPIO_MODER_MODER9_0);
        SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR9_0);

        SET_BIT(GPIOB->MODER, GPIO_MODER_MODER5_0);
        SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR5_0);

        SET_BIT(GPIOB->MODER, GPIO_MODER_MODER12_0);
        SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR12_0);

        SET_BIT(GPIOB->MODER, GPIO_MODER_MODER13_0);
        SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR13_0);

        SET_BIT(GPIOB->MODER, GPIO_MODER_MODER15_0);
        SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR15_0);

        SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0_1);
        SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR9_1);
        SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR12_1);
}
void ITR_Ini(void)
{
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

        MODIFY_REG(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0_Msk, SYSCFG_EXTICR1_EXTI0_PC);        // Настройка мультиплексора на вывод линии прерывания EXTI13 на PC13
        SET_BIT(EXTI->IMR, EXTI_IMR_MR0);                                                        // Настройка маскирования 9 линии
        SET_BIT(EXTI->RTSR, EXTI_RTSR_TR0);                                                      // Настройка детектирования нарастающего фронта 13 линии
        SET_BIT(EXTI->FTSR, EXTI_FTSR_TR0);                                                      // Настройка детектирования спадающего фронта 13 линии
        NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); // Установка 0 приоритета прерывания для вектора EXTI15_10
        NVIC_EnableIRQ(EXTI0_IRQn);

        MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI9_Msk, SYSCFG_EXTICR3_EXTI9_PC);
        SET_BIT(EXTI->IMR, EXTI_IMR_MR9);
        SET_BIT(EXTI->RTSR, EXTI_RTSR_TR9);
        SET_BIT(EXTI->FTSR, EXTI_FTSR_TR9);
        NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(EXTI9_5_IRQn);

        MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI12_Msk, SYSCFG_EXTICR4_EXTI12_PC);
        SET_BIT(EXTI->IMR, EXTI_IMR_MR12);
        SET_BIT(EXTI->RTSR, EXTI_RTSR_TR12);
        SET_BIT(EXTI->FTSR, EXTI_FTSR_TR12);
        NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(EXTI15_10_IRQn);
}
void SysTick_Init(void)
{
        CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                     // На всякий случай предварительно выключим счётчик
        SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);                                      // Разрешаем прерывание по системному таймеру
        SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);                                    // Источник тактирования будет идти из AHB без деления
        MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 179999 << SysTick_LOAD_RELOAD_Pos); // Будет тикать с частотой 1 кГц и вызывать прерывания
        MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 179999 << SysTick_VAL_CURRENT_Pos);  // Начнём считать со значения 179 999
        SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                       // Включим счётчик
}