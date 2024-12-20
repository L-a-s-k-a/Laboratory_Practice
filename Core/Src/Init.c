#include "init.h"
void RCC_Ini(void)
{
    /* Предварительная очистка регистров RCC */
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
    /* Настройка главного регистра RCC */
    SET_BIT(RCC->CR, RCC_CR_HSEON); // Запускаем внешний кварцевый резонатор
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET); // Ждём пока он запустится
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); // Сбросим бит байпаса в 0, если вдруг там что-то лежит
    SET_BIT(RCC->CR, RCC_CR_CSSON);    // Запустим Clock detector

    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); // Выставляем предделитель входной частоты PLL на 4
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7);
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); // Настраиваем предделитель получившейся частоты после умножения. Иными словами, получаем итоговую частоту PLL
    SET_BIT(RCC->CR, RCC_CR_PLLON);                // Запустим PLL
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY)); // Ждём запуска PLL

    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);           // Выбираем PLL в качестве System Clock
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);      // Предделитель AHB, без делителя
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);    // Предделитель APВ1, делим на 4
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);    // Предделитель APВ2, делим на 2
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk); // Предделитель на выходе MCO2 (PC9) = 5
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);                           // Настраиваем на выход MCO2 - System clock

    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}
void GPIO_Ini(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN); // Включение тактирования портов GPIOB и GPIOC
    
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD13_1);                       // Настройка порта РС13 на вход со стяжкой (Pull-down)
    //PB0 - green
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);  
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    SET_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0); //0x1 - 01  10 &= ~01 => 10 &= 10 = 10
    SET_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0); 
    //PB7 - blue
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);
    //PB14 - red
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14_0); 
    //PE2,4,5,6,3 PF8
    //PE2
    SET_BIT(GPIOE->MODER, GPIO_MODER_MODE2_0);
    CLEAR_BIT(GPIOE->OTYPER, GPIO_OTYPER_OT_2);
    SET_BIT(GPIOE->OSPEEDR, GPIO_OSPEEDER_OSPEEDR2_0);
    CLEAR_BIT(GPIOE->PUPDR, GPIO_PUPDR_PUPDR2_0);
    //PE4
    SET_BIT(GPIOE->MODER, GPIO_MODER_MODE4_0);
    CLEAR_BIT(GPIOE->OTYPER, GPIO_OTYPER_OT_4);
    SET_BIT(GPIOE->OSPEEDR, GPIO_OSPEEDER_OSPEEDR4_0);
    CLEAR_BIT(GPIOE->PUPDR, GPIO_PUPDR_PUPDR4_0);
    //PE5
    SET_BIT(GPIOE->MODER, GPIO_MODER_MODE5_0);
    CLEAR_BIT(GPIOE->OTYPER, GPIO_OTYPER_OT_5);
    SET_BIT(GPIOE->OSPEEDR, GPIO_OSPEEDER_OSPEEDR5_0);
    CLEAR_BIT(GPIOE->PUPDR, GPIO_PUPDR_PUPDR5_0);
    //PE6
    SET_BIT(GPIOE->MODER, GPIO_MODER_MODE6_0);
    CLEAR_BIT(GPIOE->OTYPER, GPIO_OTYPER_OT_6);
    SET_BIT(GPIOE->OSPEEDR, GPIO_OSPEEDER_OSPEEDR6_0);
    CLEAR_BIT(GPIOE->PUPDR, GPIO_PUPDR_PUPDR6_0);
    //PE3
    SET_BIT(GPIOE->MODER, GPIO_MODER_MODE3_0);
    CLEAR_BIT(GPIOE->OTYPER, GPIO_OTYPER_OT_3);
    SET_BIT(GPIOE->OSPEEDR, GPIO_OSPEEDER_OSPEEDR3_0);
    CLEAR_BIT(GPIOE->PUPDR, GPIO_PUPDR_PUPDR3_0);
    //PF8
    SET_BIT(GPIOF->MODER, GPIO_MODER_MODE8_0);
    CLEAR_BIT(GPIOF->OTYPER, GPIO_OTYPER_OT_8);
    SET_BIT(GPIOF->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);
    CLEAR_BIT(GPIOF->PUPDR, GPIO_PUPDR_PUPDR8_0); 
}
void EXTI_ITR_Ini(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); // Включение тактирования периферии SYSCFG
    
    MODIFY_REG(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0_Msk, SYSCFG_EXTICR1_EXTI0_PC);
    SET_BIT(EXTI->IMR, EXTI_IMR_MR0);
    SET_BIT(EXTI->RTSR, EXTI_IMR_MR0);
    SET_BIT(EXTI->FTSR, EXTI_IMR_MR0);
    NVIC_SetPriority(EXTI0_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI0_IRQn);// Включение прерывания по вектору EXTI0_IRQn
}
void SysTick_Init(void)
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);  // Защита от помех (от включение)
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);   // Разрешаем прерывание по системному таймеру
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk); // Источник тактирования будет идти из AHB без деления
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk,
               179999 << SysTick_LOAD_RELOAD_Pos); // Будет тикать с частотой 1 кГц и вызывать прерывания
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk,
               179999 << SysTick_VAL_CURRENT_Pos);   // Начнём считать со значения 179 999
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); // Включим счётчик
}