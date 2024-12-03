#include "init.h"

void GPIO_Ini() {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

    // настройка пинов светодиодов PC 7-12
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE7_0);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED7_0);
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE8_0);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED8_0);
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE9_0);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_0);
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE10_0);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED10_0);
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE11_0);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED11_0);
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE12_0);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED12_0);
}

void RCC_Ini() {
    // очистка регистров, битов
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
    
    // включаем внешний источник таковой частоты
    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
    SET_BIT(RCC->CR, RCC_CR_CSSON);

    // настройка блока PLL
    // устанавливаем входной предделитель M = 2
    // устанавливаем множитель N = 180
    // сбрасываем выходной предделитель P = 2
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 |
    RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7);
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk);

    // настройка конфигураций RCC
    // AHB предделитель = 1
    // APB1 предделитель = 4
    // APB2 предделитель = 2
    // MCO2 предделитель = 5 на выходе PC9
    // настройка задержки внутренней памяти
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY));
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk);
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}

void ITR_Ini() {
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    // настройка мультиплексора на вывод линии прерывания EXTI13 на PC13
    // настройка маскирования 13 линии и детектирования фронта
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, SYSCFG_EXTICR4_EXTI13_PC);
    SET_BIT(EXTI->IMR, EXTI_IMR_MR13);
    SET_BIT(EXTI->RTSR, EXTI_IMR_MR13);

    // установка приоритета для вектора EXTI15_10 и его включение
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    
    MODIFY_REG(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0_Msk, SYSCFG_EXTICR1_EXTI0_PC);
    SET_BIT(EXTI->IMR, EXTI_IMR_MR0);
    SET_BIT(EXTI->RTSR, EXTI_IMR_MR0);
    SET_BIT(EXTI->FTSR, EXTI_IMR_MR0);
    NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI0_IRQn);
    
    MODIFY_REG(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI3_Msk, SYSCFG_EXTICR1_EXTI3_PC);
    SET_BIT(EXTI->IMR, EXTI_IMR_MR3);
    SET_BIT(EXTI->RTSR, EXTI_IMR_MR3);
    SET_BIT(EXTI->FTSR, EXTI_IMR_MR3);
    NVIC_SetPriority(EXTI3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI3_IRQn);
}

void SysTick_Ini() {
    // выключаем счетчик
    // разрешаем прерываение по стистемному таймеру
    // тактирование из шины AHB без деления
    // значение с которого начинается счёт, эквивалентное 1 кГц
    // очистка поля и включение счётчика, только после настройки всего
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 179999 << SysTick_LOAD_RELOAD_Pos);
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 179999 << SysTick_VAL_CURRENT_Pos);
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}