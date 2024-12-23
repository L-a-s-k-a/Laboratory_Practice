#include "../Inc/init.h"

void GPIO_Init_Self(void)
{
    //set LED 1 (pin14)
    RCC_AHB1ENR |= RCC_GPIOB_EN | RCC_GPIOC_EN; //Включение тактирования портов GPIOB и GPIOC
    *(uint32_t*)(0x40020400UL) |= 0x10000000UL;//Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x0000UL; //Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x10000000UL; //Настройка скорости работы 14-го пина GPIOB на среднюю
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x0000UL; //Отключение PU/PD резисторов для 14-го пина GPIOB
    *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
    //set LED 2 (pin0)
    GPIOB_MODER |= GPIOB_MODE_PIN0_OUT; //Настройка работы 0-го пина GPIOB в режиме вывода сигнала (Output mode)
    GPIOB_OTYPER |= GPIOB_OTYPE_PIN0_PP; //Настройка на Push-Pull работу 0-го пина GPIOB (Output Push-Pull)
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN0_MID; //Настройка скорости работы 0-го пина GPIOB на среднюю
    GPIOB_PUPDR |= GPIOB_PUPDR_PIN0_NOPUPD; //Отключение PU/PD резисторов для 0-го пина GPIOB
    GPIOB_BSRR |= GPIOB_BSRR_PIN0_RESET;//выключение светодиода 0-го пина GPIOB
}
void GPIO_Init_CMSIS(void)
{
    //SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); //Включение тактирования портов GPIOB и GPIOC
    //set LED 3 (pin7)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0); //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT7);//Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);//Настройка скорости работы 7-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);//Отключение PU/PD резисторов для 7-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//сброс BSRR, выключение светодиода 7-го пина GPIOB
    //set LED 4 (pin8)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE8_0); //Настройка работы 8-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT8);//Настройка на Push-Pull работу 8-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);//Настройка скорости работы 8-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR8_0);//Отключение PU/PD резисторов для 8-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//сброс BSRR, выключение светодиода 8-го пина GPIOB


    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER9_1); 
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk); //Настраиваем пин на максимальную скорость работы 
    MODIFY_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL9_Msk, 0x0); //Выбираем тип альтернативной функции – Выход MCO2
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
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); //Сбросим бит байпаса в 0, если вдруг там что-то лежит 
    SET_BIT(RCC->CR, RCC_CR_CSSON); //Запустим Clock detector
    
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); //Выставляем предделитель входной частоты PLL на 4
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7); //Настраиваем умножение частоты, полученной после деления (частоты VCO) на х180 
    //CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); //Настраиваем предделитель получившейся частоты после умножения. Иными словами, получаем итоговую частоту PLL 
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0);
    SET_BIT(RCC->CR, RCC_CR_PLLON); //Запустим PLL 
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY)); //Ждём запуска PLL
    
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); //Выбираем PLL в качестве System Clock 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); //Предделитель AHB без делителя
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4); //Предделитель APВ1, делим на 4 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2); //Предделитель APВ2, делим на 2 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk); //Предделитель на выходе MCO2 (PC9) = 5 
    //MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_1 | RCC_CFGR_MCO2PRE_2); //Предделитель на выходе MCO2 (PC9) = 4 
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2); //Настраиваем на выход MCO2 - System clock
    //количество циклов задержки памяти на 6 циклов CPU
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS); 
}

void ITR_init(void){ 
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); //Включение тактирования периферии SYSCFG
    NVIC_SetPriorityGrouping(0);//установка типа группировки без подприоритетов
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, SYSCFG_EXTICR4_EXTI13_PC); //Настройка мультиплексора на вывод линии прерывания EXTI13 на PC13 
    SET_BIT(EXTI->IMR, EXTI_IMR_MR13); //Настройка маскирования 13 линии 
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13); //Настройка детектирования нарастающего фронта 13 линии 
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR13); //Настройка детектирования спадающего фронта 13 линии 
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0)); //Установка 2 приоритета прерывания для вектора EXTI15_10 
    NVIC_EnableIRQ(EXTI15_10_IRQn); //Включение прерывания по вектору EXTI15_10 
} 

void SysTick_Init(void){ 
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); //предварительно выключим счётчик 
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk); //Разрешаем прерывание по системному таймеру 
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk); //Источник тактирования будет идти из AHB без деления 
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 179999 << SysTick_LOAD_RELOAD_Pos); //Будет тикать с частотой 1 кГц и вызывать прерывания 
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 179999 << SysTick_VAL_CURRENT_Pos); //Начнём считать со значения 179 999 
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); //Включим счётчик 
} 