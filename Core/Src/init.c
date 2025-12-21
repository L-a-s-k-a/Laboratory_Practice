#include "C:\Users\parni\Git_repos\Laboratory_Practice\Core\Inc\init.h"

uint16_t FrequencyTickCount = 0;
void SysTick_Init(void)
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                      // Выключим счетчик
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);                                       // Разрешаем прерывание по системному таймеру
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);                                     // Источник тактирования AHB без делителя
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, 95999 << SysTick_LOAD_RELOAD_Pos);   // Значение с которого начинается счёт, эквивалентное 1 кГц 
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, 95999 << SysTick_VAL_CURRENT_Pos);    // Очистка поля
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                        // Включим счетчик
}

void ITR_Init(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);                    //Включение тактирования периферии SYSCFG 

    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI12_Msk, 
    SYSCFG_EXTICR4_EXTI12_PC);                                      //Настройка мультиплексора на вывод линии прерывания EXTI12 на PC12

    // настройка EXTI регистров
    SET_BIT(EXTI->IMR, EXTI_IMR_MR12);                              //Настройка маскирования 12 линии 
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR12);                            //Настройка детектирования нарастающего фронта 12 линии 
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR12);                            //Настройка детектирования спадающего фронта 12 линии 
    NVIC_SetPriority(EXTI15_10_IRQn, 
    NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));         //Установка 0 приоритета прерывания для вектора EXTI15_10 
    NVIC_EnableIRQ(EXTI15_10_IRQn);                                 //Включение прерывания по вектору EXTI15_10 
}

void RCC_Init(void)
{
     /* Предварительная очистка регистров RCC */ 
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80UL); 
    CLEAR_REG(RCC->CFGR); 
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON); 
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON); 
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); 
 
    /* Настройка главного регистра RCC */ 
    SET_BIT(RCC->CR, RCC_CR_HSION);                              
    while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET);
 
    /*Настройка регистра PLLCFGR*/
    CLEAR_REG(RCC->PLLCFGR);                                        // Очищаем регистр PLLCFGR
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSI);                  // Источник тактирования HSI
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_3);                      // Деление источника тактирования на 8
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_5); // Настрока умножения на 96
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0);                    // Делитель для выхода PLL на 2
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);                      // Делитель для USB на 15

    /*Настройка регистра CFGR*/
    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);                            // Источник системного тактирования выход PLL 96мгц
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV1);                       // Делитель AHB1  1
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV2);                        // Делитель APB1  2 (48 МГц)
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE2_DIV1);                        // Делитель APB2  1 (96 МГц)
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);  
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2_0 | RCC_CFGR_MCO2_1);        // Выход MCO2 Sysclk
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_1);    // Делитель для MCO2 4

    /*Настрйока частоты FLASH-памяти*/
    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_3WS);

    /*Включение блока PLL*/
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);

}


void GPIO_Init(void)
{
    // включаем тактирование портов С и B
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // инициализируем 1-ый светодиод
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER1_0);          // Настройка работы 1-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT1);           // Настройка на PushPull работу 1-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR1_0);   // Настройка скорости работы 1-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR1);          // Отключение PU/PD резисторов для 1-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR1);                 // выключаем светодиод

    // инициализируем 2-ой светодиод
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER15_0);          // Настройка работы 15-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT15);           // Настройка на PushPull работу 15-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR15_0);   // Настройка скорости работы 15-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR15);          // Отключение PU/PD резисторов для 15-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);                 // выключаем светодиод

    // инициализируем 3-ий светодиод
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER14_0);          // Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT14);           // Настройка на PushPull работу 14-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);   // Настройка скорости работы 14-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14);          // Отключение PU/PD резисторов для 14-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);                 // выключаем светодиод

    // инициализируем 4-ий светодиод
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER13_0);          // Настройка работы 13-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT13);           // Настройка на PushPull работу 13-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR13_0);   // Настройка скорости работы 13-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR13);          // Отключение PU/PD резисторов для 13-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13);                 // выключаем светодиод
    
    // инициализируем 5-ий светодиод
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER10_0);          // Настройка работы 10-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT10);           // Настройка на PushPull работу 10-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR10_0);   // Настройка скорости работы 10-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR10);          // Отключение PU/PD резисторов для 10-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);                 // выключаем светодиод
    
    // инициализируем 6-ий светодиод
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER5_0);          // Настройка работы 5-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT5);           // Настройка на PushPull работу 5-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR5_0);   // Настройка скорости работы 5-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR5);          // Отключение PU/PD резисторов для 5-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR5);                 // выключаем светодиод
}

// выбор включенного светодиода
void SetLEDState(bool ledB1, bool ledB15, bool ledB14, bool ledB13, bool ledB10, bool ledB4)
{
    if (ledB1)
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS1);
    else
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR1);
    if (ledB15)
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15);
    else
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);

    if (ledB14)
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
    else
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);

    if (ledB13)
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13);
    else
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13);

    if (ledB10)
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS10);
    else
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);

    if (ledB4)
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5);
    else
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR5);
}

void first_led_mode(uint8_t freq_mode)
{
    if (freq_mode == 0)
    {
        if (FrequencyTickCount <= freq_0_4Hz)
        {
            SetLEDState(1, 0, 0, 0, 0, 0);
        }
        else if (FrequencyTickCount <= 2 * freq_0_4Hz)
        {
            SetLEDState(0, 1, 0, 0, 0, 0);
        }
        else if (FrequencyTickCount <= 3 * freq_0_4Hz)
        {
            SetLEDState(0, 0, 1, 0, 0, 0);
        }
        else if (FrequencyTickCount <= 4 * freq_0_4Hz)
        {
            SetLEDState(0, 0, 0, 1, 0, 0);
        }
        else if (FrequencyTickCount <= 5 * freq_0_4Hz)
        {
            SetLEDState(0, 0, 0, 0, 1, 0);
        }
        else if (FrequencyTickCount <= 6 * freq_0_4Hz)
        {
            SetLEDState(0, 0, 0, 0, 0, 1);
        }
        else if (7 * freq_0_4Hz < FrequencyTickCount)
        {
            FrequencyTickCount = 0;
            SetLEDState(0, 0, 0, 0, 0, 0);
        }
    }
    else if (freq_mode == 1)
    {
        if (FrequencyTickCount <= freq_1_9Hz)
        {
            SetLEDState(1, 0, 0, 0, 0, 0);
        }
        else if (FrequencyTickCount <= 2 * freq_1_9Hz)
        {
            SetLEDState(0, 1, 0, 0, 0, 0);
        }
        else if (FrequencyTickCount <= 3 * freq_1_9Hz)
        {
            SetLEDState(0, 0, 1, 0, 0, 0);
        }
        else if (FrequencyTickCount <= 4 * freq_1_9Hz)
        {
            SetLEDState(0, 0, 0, 1, 0, 0);
        }
        else if (FrequencyTickCount <= 5 * freq_1_9Hz)
        {
            SetLEDState(0, 0, 0, 0, 1, 0);
        }
        else if (FrequencyTickCount <= 6 * freq_1_9Hz)
        {
            SetLEDState(0, 0, 0, 0, 0, 1);
        }
        else if (7 * freq_1_9Hz < FrequencyTickCount)
        {
            FrequencyTickCount = 0;
            SetLEDState(0, 0, 0, 0, 0, 0);
        }
    }
    else if (freq_mode == 2)
    {
        if (FrequencyTickCount <= freq_2_6Hz)
        {
            SetLEDState(1, 0, 0, 0, 0, 0);
        }
        else if (FrequencyTickCount <= 2 * freq_2_6Hz)
        {
            SetLEDState(0, 1, 0, 0, 0, 0);
        }
        else if (FrequencyTickCount <= 3 * freq_2_6Hz)
        {
            SetLEDState(0, 0, 1, 0, 0, 0);
        }
        else if (FrequencyTickCount <= 4 * freq_2_6Hz)
        {
            SetLEDState(0, 0, 0, 1, 0, 0);
        }
        else if (FrequencyTickCount <= 5 * freq_2_6Hz)
        {
            SetLEDState(0, 0, 0, 0, 1, 0);
        }
        else if (FrequencyTickCount <= 6 * freq_2_6Hz)
        {
            SetLEDState(0, 0, 0, 0, 0, 1);
        }
        else if (7 * freq_2_6Hz < FrequencyTickCount)
        {
            FrequencyTickCount = 0;
            SetLEDState(0, 0, 0, 0, 0, 0);
        }
    }
}

void second_led_mode(uint8_t freq_mode)
{
    if (freq_mode == 0)
    {
        if (FrequencyTickCount <= freq_0_3Hz)
        {
            SetLEDState(1, 0, 0, 1, 0, 0);
        }
        else if (FrequencyTickCount <= 2 * freq_0_3Hz)
        {
            SetLEDState(0, 1, 0, 0, 1, 0);
        }
        else if (FrequencyTickCount <= 3 * freq_0_3Hz)
        {
            SetLEDState(0, 0, 1, 0, 0, 1);
        }
        else if (3 * freq_0_3Hz < FrequencyTickCount)
        {
            FrequencyTickCount = 0;
            SetLEDState(0, 0, 0, 0, 0, 0);
        }
    }
    else if (freq_mode == 1)
    {
        if (FrequencyTickCount <= freq_1_6Hz)
        {
            SetLEDState(1, 0, 0, 1, 0, 0);
        }
        else if (FrequencyTickCount <= 2 * freq_1_6Hz)
        {
            SetLEDState(0, 1, 0, 0, 1, 0);
        }
        else if (FrequencyTickCount <= 3 * freq_1_6Hz)
        {
            SetLEDState(0, 0, 1, 0, 0, 1);
        }
        else if (3 * freq_1_6Hz < FrequencyTickCount)
        {
            FrequencyTickCount = 0;
            SetLEDState(0, 0, 0, 0, 0, 0);
        }
    }
    else if (freq_mode == 2)
    {
        if (FrequencyTickCount <= freq_2_3Hz)
        {
            SetLEDState(1, 0, 0, 1, 0, 0);
        }
        else if (FrequencyTickCount <= 2 * freq_2_3Hz)
        {
            SetLEDState(0, 1, 0, 0, 1, 0);
        }
        else if (FrequencyTickCount <= 3 * freq_2_3Hz)
        {
            SetLEDState(0, 0, 1, 0, 0, 1);
        }
        else if (3 * freq_2_3Hz < FrequencyTickCount)
        {
            FrequencyTickCount = 0;
            SetLEDState(0, 0, 0, 0, 0, 0);
        }
    }
}
    
