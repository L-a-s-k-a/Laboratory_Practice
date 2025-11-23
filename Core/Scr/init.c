#include "../Inc/init.h"

uint16_t freq[3][3] = {{2500, 769, 500}, {1667, 265, 455}, {1250, 526, 400}};
bool led1_shine = false;
uint16_t TickCounter = 0;
uint16_t PrevDelay = 0;
bool led1 = false;
bool led2 = false;
bool led3 = false;
bool led4 = false;
bool led5 = false;
bool led6 = false;

void GPIO_INIT(void)
{
    // Включение всех используемых шин тактирования GPIO: A, B и С
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
    // Настройка пинов светодиодов: РС0, РС1, РВ0, РА4, РА1 и РА0
    // установка пинов на режим вывода сигнала
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0);
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE4_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    // Настройка типа выхода пинов диодов на режим Pull-Push
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT4);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT0);
    // Настройка скорости работы пинов светодиодов на среднюю скорость
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR1_0);
    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR1_0 | GPIO_OSPEEDER_OSPEEDR4_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    // Отключение подтяжек/стяжек для пинов светодиодов
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1);
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1 | GPIO_PUPDR_PUPDR4);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0);
    // Подаем низкий уровень сигнала на светодиоды перед началом работы
    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
    SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR0 | GPIO_BSRR_BR1 | GPIO_BSRR_BR4);
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
    // Настройка пинов для работы кнопок: РС2 и РС3
    // Явная настройка пинов кнопки на выход
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE3 | GPIO_MODER_MODE2);
    // Включение подтяжки на пины кнопок
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR3_0);
    // Настройка порта PC9 для измерения частоты тактирования
    // Настройка режима работы пина на альтернативное использование
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE9_1);
    // Настройка пина на макимальную скорость работы
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9);
    // Выбор типа алтернативной функции - МСО2
    MODIFY_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL9, 0X00UL);
}

void RCC_INIT(void)
{
    // Предварительная очистка регистров RCC
    //  Очистка битов HSITRIM и установка значения
    //  регулирования отклонения на 10000000
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80UL);
    // Очистка регистра конфигурации таймера
    CLEAR_REG(RCC->CFGR);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET)
        ;
    // Очистка бита отключение PLL до начала работы
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET)
        ;
    // Отключение внешнего источника тактирования и
    // системы слежения за его частотой
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET)
        ;
    // Выключения обхождения внешнего тактирования
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);

    // Настройка главного регистра RCC
    //  Включение внутреннего источника тактирования
    SET_BIT(RCC->CR, RCC_CR_HSION);
    while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET)
        ;

    // Настройка регистра конфигурации PLL
    //  Очищаем регистр PLLCFGR
    CLEAR_REG(RCC->PLLCFGR);
    // Использование внутреннего источника тактирования
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSI);
    // Деление частоты источника тактирования на 8
    // 16/8 = 2 МГц
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_3);
    // Настрока умножения частоты PLL в блоке VCO на 96
    // 2*96 = 192 МГц
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_5);
    // Делитель для выхода VCO на 2
    // 192/2 = 96 МГц
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0);
    // Делитель для выхода сигнала на USB порт на 2
    // 96/2 = 48 МГц
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);

    // Настройка регистра концигурации таймера
    // Источник системного тактирования - PLL
    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);
    // Делитель частоты на шине AHB -> 1
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV1);
    // Делитель частоты на низкочастотной шине AHB1 -> 2
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV2);
    // Делитель частоты на высокочастотной шине AHB2 -> 1
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE2_DIV1);
    // Подача на MCO2 сигнала с системного таймера
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);
    // Деление частоты, подаваемой на МСО2 на 4
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_2); // Делитель для MCO2 -> 4

    // Настрйока частоты FLASH-памяти
    //  Выставление задержки работы FLASH-памяти на 3 такта ожидания
    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_3WS);
    // Включение блока PLL
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET)
        ;
}

void IQR_INIT(void)
{
    // Включение тактирование периферии
    // от системаного таймера
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    // Обозначение снятие внешеного сигнала с лини1 PC3 и PC2
    MODIFY_REG(SYSCFG->EXTICR[0],
               SYSCFG_EXTICR1_EXTI3_Msk, SYSCFG_EXTICR1_EXTI3_PC);
    MODIFY_REG(SYSCFG->EXTICR[0],
               SYSCFG_EXTICR1_EXTI2_Msk, SYSCFG_EXTICR1_EXTI2_PC);

    // Настройка регистров EXTI
    // Включение маскирования прерываний
    SET_BIT(EXTI->IMR, EXTI_IMR_IM3 | EXTI_IMR_IM2);
    // Выключение маскирования запроса на событие прерывания
    CLEAR_BIT(EXTI->EMR, EXTI_EMR_EM3 | EXTI_EMR_EM2);
    // Включение отслеживания нажатия кнопки по фронту
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR3 | EXTI_RTSR_TR2);
    // Включение отслеживания нажатия кнопки по спаду
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR3 | EXTI_FTSR_TR2);

    // Настройка регистров NVIC для определения приоритетов
    // Автоматическое распределение приоритетов
    NVIC_SetPriority(EXTI3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_SetPriority(EXTI2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(EXTI2_IRQn);
}

void SYSTIMER_INIT(void)
{
    // Настройка системеного таймера
    // Выключение системного таймера для настройки
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
    // Разрешение прерываний при достижении нуля счетчика
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);
    // Источник тактирования AHB без делителя
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);
    // Значение начала счета, эквивалентное 1 кГц
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk,
               95999 << SysTick_LOAD_RELOAD_Pos);
    // Сброс бита COUNTFLAG с помощью VAL
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk,
               95999 << SysTick_VAL_CURRENT_Pos);
    // Включение системного таймера
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);
}

void working_mode(uint8_t num, uint8_t LED_OPTION[2][6])
{
    uint8_t smth;
    if (num == 0)
        smth = 1;
    else
        smth = num;

    uint16_t shine = LED_OPTION[1][num];
    uint16_t mode = LED_OPTION[0][num];
    uint16_t CurrentFreq = freq[LED_OPTION[1][num]][LED_OPTION[0][num]];
    switch (num)
    {
    case 0:
        led1 = !led1;
        GPIOA->BSRR |= GPIO_BSRR_BS_0;
        Delay(CurrentFreq);
        led1 = !led1;
        GPIOA->BSRR |= GPIO_BSRR_BR_0;
        Delay(CurrentFreq);
        break;
    case 1:
        led2 = !led2;
        GPIOA->BSRR |= GPIO_BSRR_BR_0;
        GPIOA->BSRR |= GPIO_BSRR_BS_1;
        Delay(CurrentFreq);
        led2 = !led2;
        GPIOA->BSRR |= GPIO_BSRR_BR_1;
        Delay(CurrentFreq);
        break;
    case 2:
        led3 = !led3;
        GPIOA->BSRR |= GPIO_BSRR_BR_1;
        GPIOA->BSRR |= GPIO_BSRR_BS_4;
        Delay(CurrentFreq);
        led3 = !led3;
        GPIOA->BSRR |= GPIO_BSRR_BR_4;
        Delay(CurrentFreq);
        break;
    case 3:
        led4 = !led4;
        GPIOA->BSRR |= GPIO_BSRR_BR_4;
        GPIOB->BSRR |= GPIO_BSRR_BS_0;
        Delay(CurrentFreq);
        led4 = !led4;
        GPIOB->BSRR |= GPIO_BSRR_BR_0;
        Delay(CurrentFreq);
        break;
    case 4:
        led5 = !led5;
        GPIOB->BSRR |= GPIO_BSRR_BR_0;
        GPIOC->BSRR |= GPIO_BSRR_BS_1;
        Delay(CurrentFreq);
        led5 = !led5;
        GPIOC->BSRR |= GPIO_BSRR_BR_1;
        Delay(CurrentFreq);
        break;
    case 5:
        led6 = !led6;
        GPIOC->BSRR |= GPIO_BSRR_BR_1;
        GPIOC->BSRR |= GPIO_BSRR_BS_0;
        Delay(CurrentFreq);
        led6 = !led6;
        GPIOC->BSRR |= GPIO_BSRR_BR_0;
        Delay(CurrentFreq);
        break;
    default:
        GPIOC->BSRR |= GPIO_BSRR_BR_0;
        break;
    }
}