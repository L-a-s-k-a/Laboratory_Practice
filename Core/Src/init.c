#include "init.h"

void GPIO_Ini(void)
{
    // инициализация С порта через прямого обрашения к памяти
    *(uint32_t*)(0x40023800UL+0x30UL)|= 0x04UL;// включаем тактирование на  С  порте 
    *(uint32_t *)(0x40020800UL + 0x00UL) |= 0x1000000UL; // C moder1 12 cvetadiod output
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE10_0 | GPIO_MODER_MODE11_0 | GPIO_MODER_MODE3_0);
    *(uint32_t *)(0x40020800UL + 0x08UL) |= 0x1000000UL; //С12 порт на средную скороть
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR10_0 | GPIO_OSPEEDER_OSPEEDR11_0 | GPIO_OSPEEDER_OSPEEDR3_0);
    *(uint32_t *)(0x40020800UL + 0x0CUL) |= 0x00UL; //С12 порт
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD10_0 | GPIO_PUPDR_PUPD11_0 | GPIO_PUPDR_PUPD3_0);

    // инициализация В порта через макросы
    SET_bit(RCC_GPIO_en, RCC_GPIOB_en); // включаем тактирование на  B  порте
    SET_bit(GPIOB_moder, GPIOB_moder_PIN7);    // B moder7 01 cvetadiod output 
    SET_bit(GPIOB_ospeed, GPIOB_ospeed_PIN7);  //B7порт на средную скороть
    SET_bit(GPIOB_pull_up_down, GPIOB_pull_up_down_PIN7); //B7 порт 0 

    // инициализация D порта через CMSIS
   SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // включаем тактирование на  D  порте через CMSIS
   SET_BIT(GPIOD->MODER, GPIO_MODER_MODE2_0);  // D moder2 01 cvetadiod output
   SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR2_0); //D2 порт на средную скороть
   SET_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD2_0); //D2 порт 0


    //настройка PC9 в режиме алтернативной функции
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER9_1);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk);
    MODIFY_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL9_Msk, 0x0);
}

void RCC_Init(void){
    //предварительная очистка регистров RCC устанавливается внутренный высокочастотный генератор
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
    CLEAR_REG(RCC->CFGR);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

    //настройка RCC_CR
    SET_BIT(RCC->CR, RCC_CR_HSEON);//запуск внешного генератора
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);//разрешение использование внешного источника
    SET_BIT(RCC->CR, RCC_CR_CSSON);// ПИн контролья стабилности внешного генератора

    // настройка RCC_PLLCFGR(регистр конфигурации фазовой автоподстройки частоты)
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);//Эта строка устанавливает источник PLL — то есть указывает, откуда PLL должна брать входную частоту
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); //предделитель PLLM2 на 4
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_6);// умножение частоты на 100
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); // Настройка предделител итоговая частота
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);

    //настройка регистора RCC_CFGR(конфигурирует систему тактирования)
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);// выбор PLL
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); //предделитель ABH, без делителя
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2); //предделитель APB1, на 2
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1); //предделитель APB2, без делителя
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_0); //предделитель на выходе MCO2(PC9)=3
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2); //настройка на выход
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_3WS);

    SET_BIT(RCC->CR, RCC_CR_PLLON);//Запуск PLL
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);
}

void IRO_INInt(void){
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
    MODIFY_REG(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI8_Msk | SYSCFG_EXTICR3_EXTI9_Msk, SYSCFG_EXTICR3_EXTI8_PB | SYSCFG_EXTICR3_EXTI9_PB);

    SET_BIT(EXTI->IMR, EXTI_IMR_IM8 | EXTI_IMR_IM9); 
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR8 | EXTI_RTSR_TR9); // прерывание по фронту
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR8 | EXTI_FTSR_TR9); // прерывание по спаду

   
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); //Установка 0 приоритета прерывания для вектора EXTI9_5
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void SysTick_Init(void)
{ 
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); //На всякий случай, предварительно, выключим счётчик 
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk); //Разрешаем прерывание по системному таймеру 
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk); //Источник тактирования будет идти из AHB без деления 
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 95999 << SysTick_LOAD_RELOAD_Pos); //Значение с которого начинается счёт, эквивалентное 1 кГц 
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 95999 << SysTick_VAL_CURRENT_Pos); //Очистка поля 
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk); //Включим счётчик 
}
    