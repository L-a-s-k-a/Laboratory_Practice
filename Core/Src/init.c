#include "../Inc/init.h"

void GPIO_Init(void)
{
    //set LED 3 (pin14)
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE8_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE9_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE10_0);

    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_8);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_9);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_10);


    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR9_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR10_0);

    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR8_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR9_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR10_0);

    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD6_1); // Настройка порта РС6 на вход со стяжкой (Pull-down)
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD10_1); // Настройка порта РС10 на вход со стяжкой (Pull-down)

}


void RCC_Init(void){
    //Очистка всех битов, участвующих в настройке
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON); 
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON); 
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

    /* Настройка главного регистра RCC */ 
    SET_BIT(RCC->CR, RCC_CR_HSEON); //Запускаем внешний кварцевый резонатор 
    while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET); //Ждём пока он запустится 
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); //Сбросим бит обхода внешнего тактирования в 0, если вдруг там что-то лежит 
    SET_BIT(RCC->CR, RCC_CR_CSSON); //Запустим Clock detector (Clock security system)
    
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); //Выставляем предделитель входной частоты PLL на 4
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7); //Настраиваем умножение частоты, полученной после деления (частоты VCO) на х180 
    //MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_0 | RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_6); //Настраиваем умножение частоты, полученной после деления (частоты VCO) на х90
    //SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0); сам устанавливал частоту
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); //Настраиваем предделитель получившейся частоты после умножения. Иными словами, получаем итоговую частоту PLL 
    SET_BIT(RCC->CR, RCC_CR_PLLON); //Запустим PLL 
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY)); //Ждём запуска PLL
    
    //Настройка регистра RCC_CFGR
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); //Выбираем PLL в качестве System Clock 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); //Предделитель AHB без делителя
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4); //Предделитель APВ1, делим на 4 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2); //Предделитель APВ2, делим на 2 
    //MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_2); //Предделитель на выходе MCO2 (PC9) = 2
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk); //Предделитель на выходе MCO2 (PC9) = 5 
    //MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_1 | RCC_CFGR_MCO2PRE_2); //Предделитель на выходе MCO2 (PC9) = 4 
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2); //Настраиваем на выход MCO2 - System clock
    //количество циклов задержки памяти на 6 циклов CPU
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS); 
}

void EXTI_ITR_Init(void){ 
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); //Включение тактирования периферии SYSCFG
    NVIC_SetPriorityGrouping(0);//установка типа группировки без подприоритетов

    /* Настройка EXTI для PC10 */
    MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI10_Msk, SYSCFG_EXTICR3_EXTI10_PC); // PC10 на EXTI10
    SET_BIT(EXTI->IMR, EXTI_IMR_MR10);                // Маска на 10 линию
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR10);              // Детектирование нарастающего фронта
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR10);            // Детектирование детектирования спадающего фронта
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0)); // Приоритет 0
    NVIC_EnableIRQ(EXTI15_10_IRQn);  

    // MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, SYSCFG_EXTICR4_EXTI13_PC); //Настройка мультиплексора на вывод линии прерывания EXTI13 на PC13 
    // SET_BIT(EXTI->IMR, EXTI_IMR_MR13); //Настройка маскирования 13 линии 
    // SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13); //Настройка детектирования нарастающего фронта 13 линии 
    // SET_BIT(EXTI->FTSR, EXTI_FTSR_TR13); //Настройка детектирования спадающего фронта 13 линии 
    // NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0)); //Установка 2 приоритета прерывания для вектора EXTI15_10 
    // NVIC_EnableIRQ(EXTI15_10_IRQn); //Включение прерывания по вектору EXTI15_10 
    
    MODIFY_REG(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI6_Msk, SYSCFG_EXTICR2_EXTI6_PC); //Настройка мультиплексора на вывод линии прерывания EXTI13 на PC6 
    SET_BIT(EXTI->IMR, EXTI_IMR_MR6); //Настройка маскирования 6 линии 
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR6); //Настройка детектирования нарастающего фронта 6 линии 
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR6); //Настройка детектирования спадающего фронта 6 линии 
    NVIC_SetPriority(EXTI9_5_IRQn , NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0)); //Установка 2 приоритета прерывания для вектора EXTI9_5 
    NVIC_EnableIRQ(EXTI9_5_IRQn); //Включение прерывания по вектору EXTI9_5
} 

void SysTick_Init(void) {
    /* Настройка системного таймера SysTick */
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);          // Выключаем счётчик
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);           // Разрешение прерывания
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);         // Источник тактирования AHB
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 179999); // Частота прерываний 1 кГц
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 179999);  // Установка текущего значения
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);            // Включение счётчика
}