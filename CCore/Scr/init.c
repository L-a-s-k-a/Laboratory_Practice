#include "../Inc/init.h"

/*Счетчик для светодиодов*/
uint16_t FrequencyTickCount = 0;

/*Переменные для проверки частот работы 1-го режима*/
bool LED_First = false;
bool LED_Second = false;
bool LED_Third = false;
bool LED_Fourth = false;
bool LED_Fifth = false;
bool LED_Sixth = false;

/*Переменные для проверки частот работы 2-го режима*/
bool LED_First_Group = false;
bool LED_Second_Group = false;
bool LED_Trird_Group = false;

void GPIO_Init(void)
{
    /*Включение тактирования на шину GPIOC*/
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

    /*Настройка работы кнопки на вход с внутренней подтяжкой PC3*/
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE3);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD3_0);

    /*Настройка портов PC10-12, PC5-6, PC8 на выход*/
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER12_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER10_0 |
                              GPIO_MODER_MODER8_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER5_0); // Настройка портов на выход
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED12_0 | GPIO_OSPEEDR_OSPEED11_0 | GPIO_OSPEEDR_OSPEED10_0 |
                                GPIO_OSPEEDR_OSPEED8_0 | GPIO_OSPEEDR_OSPEED6_0 | GPIO_OSPEEDR_OSPEED5_0); // Настройка частоты на портах (средняя)
    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12 | GPIO_BSRR_BR11 | GPIO_BSRR_BR10 |
                             GPIO_BSRR_BR8 | GPIO_BSRR_BR6 | GPIO_BSRR_BR5); // Выключение светодиодов (если включены)

    /*Настройка порта PC9 на MCO2*/
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER9_1);          // Настраиваем пин на альтернативный режим
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9);       // Настраиваем пин на максимальную скорость работы
    MODIFY_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL9, 0x00UL); // Выбираем тип альтернативной функции – Выход MCO2
}

void RCC_Init(void)
{
    /*Предварительная очистка регистров RCC */
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80UL);                    // Очистка битов HSITRIM и установка 1000 0000
    CLEAR_REG(RCC->CFGR);                                           // Очистка регистра CFGR
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);             // Проверка очистки регистра CR
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);                               // Очистка бита PLLON в регистре CR
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);              // Проверка очистки бита PLLON
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);                // Очистка битов HSEON, CSSON в регистре CR
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);              // Проверка очистки битов HSEON, CSSON
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);                              // Очистка бита HSEBYP в регистре CR

    /*Настройка главного регистра RCC */
    SET_BIT(RCC->CR, RCC_CR_HSION);                                 // Запускаем HSI
    while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET);              // Ждём пока он запустится

    /*Настройка регистра PLLCFGR*/
    CLEAR_REG(RCC->PLLCFGR);                                        // Очищаем регистр PLLCFGR
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSI);                  // Источник тактирования HSE
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_3);                      // Деление источника тактирования на 8
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_5); // Настрока умножения на 96
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0);                    // Делитель для выхода PLL(PLLCLK) на 4
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);                      // Делитель для USB на 15

    /*Настройка регистра CFGR*/
    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);                            // Источник системного тактирования -> выход PLL
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV1);                       // Делитель AHB1 -> 0
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV2);                        // Делитель APB1 -> 2 (48 МГц)
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE2_DIV1);                        // Делитель APB2 -> 1 (96 МГц)
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);                            // Выход MCO2 -> Sysclk
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_1);    // Делитель для MCO2 -> 4

    /*Настрйока частоты FLASH-памяти*/
    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_3WS);

    /*Включение блока PLL*/
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);
}

void IQR_Init(void)
{
    /*Включение тактирование периферии SYSCFG*/
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    /*Обозначение снятие внешеного сигнала с линии PC3*/
    SET_BIT(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI3_PC);

    /*Настройка регистров EXTI*/
    SET_BIT(EXTI->IMR, EXTI_IMR_IM3);   // Включение маскирование
    CLEAR_BIT(EXTI->EMR, EXTI_EMR_EM3); // Отключение генерации события
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR3); // Настройка отслеживания по спаду (кнопка была отпущена)
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR3); // Настройка отслеживания по фронту (кнопка была нажата)

    /*Настройка регистров NVIC*/
    NVIC_SetPriority(EXTI3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); // Выбор приоретета прерывания
    NVIC_EnableIRQ(EXTI3_IRQn);                                                          // Разрешение прерывания
}

void SysTick_Init(void)
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                    // Выключим счетчик
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);                                     // Разрешаем прерывание по системному таймеру
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);                                   // Источник тактирования будет идти из AHB без деления
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 95999 << SysTick_LOAD_RELOAD_Pos); // Значение, с которого начинается счет, эквивалентное 1 кГц
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 95999 << SysTick_VAL_CURRENT_Pos);  // Очистка поля
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                      // Включим счетчик
}

void Mode_Flicker_First(uint8_t Frequency)
{
    LED_First_Group = false;
    LED_Second_Group = false;
    LED_Trird_Group = false;
    switch (Frequency)
    {
    /*Мерцание светодиодов с частотой 0.4 Гц*/
    case 0:
        if (FrequencyTickCount <= Frequency_First_Mode_0_4Hz)
        {
            LED_Sixth = false;
            LED_Second = false;
            LED_Third = false;
            LED_Fourth = false;
            LED_Fifth = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR8);
            LED_First = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
        }
        else if (FrequencyTickCount <= 2 * Frequency_First_Mode_0_4Hz)
        {
            LED_First = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12);
            LED_Second = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10);
        }
        else if (FrequencyTickCount <= 3 * Frequency_First_Mode_0_4Hz)
        {
            LED_Second = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR10);
            LED_Third = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11);
        }
        else if (FrequencyTickCount <= 4 * Frequency_First_Mode_0_4Hz)
        {
            LED_Third = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR11);
            LED_Fourth = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS5);
        }
        else if (FrequencyTickCount <= 5 * Frequency_First_Mode_0_4Hz)
        {
            LED_Fourth = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR5);
            LED_Fifth = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS6);
        }
        else if (FrequencyTickCount <= 6 * Frequency_First_Mode_0_4Hz)
        {
            LED_Fifth = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR6);
            LED_Sixth = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS8);
        }
        else if (6 * Frequency_First_Mode_0_4Hz <= FrequencyTickCount)
        {
            FrequencyTickCount = 0;
        }
        break;
    /*Мерцание светодиодов с частотой 1.9 Гц*/
    case 1:
        if (FrequencyTickCount <= Frequency_First_Mode_1_9Hz)
        {
            LED_Sixth = false;
            LED_Second = false;
            LED_Third = false;
            LED_Fourth = false;
            LED_Fifth = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR8);
            LED_First = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
        }
        else if (FrequencyTickCount <= 2 * Frequency_First_Mode_1_9Hz)
        {
            LED_First = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12);
            LED_Second = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10);
        }
        else if (FrequencyTickCount <= 3 * Frequency_First_Mode_1_9Hz)
        {
            LED_Second = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR10);
            LED_Third = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11);
        }
        else if (FrequencyTickCount <= 4 * Frequency_First_Mode_1_9Hz)
        {
            LED_Third = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR11);
            LED_Fourth = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS5);
        }
        else if (FrequencyTickCount <= 5 * Frequency_First_Mode_1_9Hz)
        {
            LED_Fourth = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR5);
            LED_Fifth = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS6);
        }
        else if (FrequencyTickCount <= 6 * Frequency_First_Mode_1_9Hz)
        {
            LED_Fifth = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR6);
            LED_Sixth = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS8);
        }
        else if (6 * Frequency_First_Mode_1_9Hz <= FrequencyTickCount)
        {
            FrequencyTickCount = 0;
        }
        break;
    /*Мерцание светодиодов с частотой 2.6 Гц*/
    case 2:
        if (FrequencyTickCount <= Frequency_First_Mode_2_6Hz)
        {
            LED_Sixth = false;
            LED_Second = false;
            LED_Third = false;
            LED_Fourth = false;
            LED_Fifth = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR8);
            LED_First = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
        }
        else if (FrequencyTickCount <= 2 * Frequency_First_Mode_2_6Hz)
        {
            LED_First = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12);
            LED_Second = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10);
        }
        else if (FrequencyTickCount <= 3 * Frequency_First_Mode_2_6Hz)
        {
            LED_Second = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR10);
            LED_Third = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11);
        }
        else if (FrequencyTickCount <= 4 * Frequency_First_Mode_2_6Hz)
        {
            LED_Third = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR11);
            LED_Fourth = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS5);
        }
        else if (FrequencyTickCount <= 5 * Frequency_First_Mode_2_6Hz)
        {
            LED_Fourth = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR5);
            LED_Fifth = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS6);
        }
        else if (FrequencyTickCount <= 6 * Frequency_First_Mode_2_6Hz)
        {
            LED_Fifth = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR6);
            LED_Sixth = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS8);
        }
        else if (6 * Frequency_First_Mode_2_6Hz <= FrequencyTickCount)
        {
            FrequencyTickCount = 0;
        }
        break;
    }
}

void Mode_Flicker_Second(uint8_t Frequency)
{
    LED_First = false;
    LED_Second = false;
    LED_Third = false;
    LED_Fourth = false;
    LED_Fifth = false;
    LED_Sixth = false;

    switch (Frequency)
    {
    /*Мерцание светодиодов с частотой 0.3 Гц*/
    case 0:
        if (FrequencyTickCount <= Frequency_Second_Mode_0_3Hz)
        {
            LED_Trird_Group = false;
            LED_Second_Group = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR11 | GPIO_BSRR_BR8);
            LED_First_Group = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12 | GPIO_BSRR_BS5);
        }
        else if (FrequencyTickCount <= 2 * Frequency_Second_Mode_0_3Hz)
        {
            LED_First_Group = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12 | GPIO_BSRR_BR5);
            LED_Second_Group = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10 | GPIO_BSRR_BS6);
        }
        else if (FrequencyTickCount <= 3 * Frequency_Second_Mode_0_3Hz)
        {
            LED_Second_Group = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR10 | GPIO_BSRR_BR6);
            LED_Trird_Group = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11 | GPIO_BSRR_BS8);
        }
        else if (3 * Frequency_Second_Mode_0_3Hz <= FrequencyTickCount)
        {
            FrequencyTickCount = 0;
        }
        break;
    /*Мерцание светодиодов с частотой 1.6 Гц*/
    case 1:
        if (FrequencyTickCount <= Frequency_Second_Mode_1_6Hz)
        {
            LED_Trird_Group = false;
            LED_Second_Group = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR11 | GPIO_BSRR_BR8);
            LED_First_Group = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12 | GPIO_BSRR_BS5);
        }
        else if (FrequencyTickCount <= 2 * Frequency_Second_Mode_1_6Hz)
        {
            LED_First_Group = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12 | GPIO_BSRR_BR5);
            LED_Second_Group = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10 | GPIO_BSRR_BS6);
        }
        else if (FrequencyTickCount <= 3 * Frequency_Second_Mode_1_6Hz)
        {
            LED_Second_Group = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR10 | GPIO_BSRR_BR6);
            LED_Trird_Group = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11 | GPIO_BSRR_BS8);
        }
        else if (3 * Frequency_Second_Mode_1_6Hz <= FrequencyTickCount)
        {
            FrequencyTickCount = 0;
        }
        break;
    /*Мерцание светодиодов с частотой 2.3 Гц*/
    case 2:
        if (FrequencyTickCount <= Frequency_Second_Mode_2_3Hz)
        {
            LED_Trird_Group = false;
            LED_Second_Group = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR11 | GPIO_BSRR_BR8);
            LED_First_Group = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12 | GPIO_BSRR_BS5);
        }
        else if (FrequencyTickCount <= 2 * Frequency_Second_Mode_2_3Hz)
        {
            LED_First_Group = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12 | GPIO_BSRR_BR5);
            LED_Second_Group = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10 | GPIO_BSRR_BS6);
        }
        else if (FrequencyTickCount <= 3 * Frequency_Second_Mode_2_3Hz)
        {
            LED_Second_Group = false;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR10 | GPIO_BSRR_BR6);
            LED_Trird_Group = true;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11 | GPIO_BSRR_BS8);
        }
        else if (3 * Frequency_Second_Mode_2_3Hz <= FrequencyTickCount)
        {
            FrequencyTickCount = 0;
        }
        break;
    }
}