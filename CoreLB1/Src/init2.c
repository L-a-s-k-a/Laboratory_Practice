#include "C:\Users\parni\Git_repos\Laboratory_Practice\CoreLB1\Inc\init2.h"



void EXT_Init(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); //Включение тактирования периферии SYSCFG 
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, 
    SYSCFG_EXTICR4_EXTI13_PC); //Настройка мультиплексора на вывод линии прерывания EXTI13 на PC13 
    SET_BIT(EXTI->IMR, EXTI_IMR_MR13); //Настройка маскирования 13 линии 
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR13); //Настройка детектирования нарастающего фронта 13 линии 
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR13); //Настройка детектирования спадающего фронта 13 линии 
    NVIC_SetPriority(EXTI15_10_IRQn, 
    NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); //Установка 0 приоритета прерывания для вектора EXTI15_10 
    NVIC_EnableIRQ(EXTI15_10_IRQn); //Включение прерывания по вектору EXTI15_10 
}

void RCC_Init(void)
{
     /* Предварительная очистка регистров RCC */ 
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
     
    /* Настройка регистров PLL 
     * Предварительная очистка регистра 
     * В качестве источника тактирования для PLL выбирается HSE 
     * Мы сначала делим входную частоту (HSE) на 4 (получаем 2 МГц), затем умножаем на 180 и 
снова делим на 2, таким образом получаем 180МГц 
     * Включаем работу PLL 
     */ 
    CLEAR_REG(RCC->PLLCFGR); 
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE); 
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, RCC_PLLCFGR_PLLM_2); //Выставляем предделитель входной частоты PLL на 4  
    MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | 
    RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7); 
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk); //Настраиваем предделитель получившейся частоты после умножения. Иными словами, получаем итоговую частоту PLL 
    SET_BIT(RCC->CR, RCC_CR_PLLON); //Запустим PLL 
    while(READ_BIT(RCC->CR, RCC_CR_PLLRDY)); //Ждём запуска PLL 
 
    /* Настройка основных конфигураций RCC 
     * В качетсве системных часов выбираем выход PLL 
     * Настраиваем предделители шин AHB и APB 
     * Настраиваем выходы MCO1 и MCO2 для внешней оценки настроенной системы тактирования 
     */ 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL); //Выбираем PLL в качестве System Clock 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1); //Предделитель AHB, без делителя 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4); //Предделитель APВ1, делим на 4 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2); //Предделитель APВ2, делим на 2 
    MODIFY_REG(RCC->CFGR, RCC_CFGR_MCO2PRE, RCC_CFGR_MCO2PRE_Msk); //Предделитель на выходе MCO2 (PC9) = 5 
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2); //Настраиваем на выход MCO2 - System clock 
    /* Настройка задержки внутренней памяти 
     * Выставление битов LATENCY регистра FLASH_ACR в позицию 5SW (6 CPU cycles).  
     * Данная настройка необходима при увеличении системной частоты тактирования свыше 20 МГц. 
Таблица 12 RM0090 
     */ 
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}


void GPIO_Init(void)
{
    // включаем тактирование портов

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // инициализируем 1-ый светодиод

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER1_0);          // Настройка работы 6-го пина GPIOС в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT1);           // Настройка на PushPull работу 6-го пина GPIOC (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR1_0);   // Настройка скорости работы 6-го пина GPIOC на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR1);          // Отключение PU/PD резисторов для 6-го пина GPIOC
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR1);               // выключаем светодиод

    // инициализируем 2-ой светодиод

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER15_0);          // Настройка работы 6-го пина GPIOС в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT15);           // Настройка на PushPull работу 6-го пина GPIOC (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR15_0);   // Настройка скорости работы 6-го пина GPIOC на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR15);          // Отключение PU/PD резисторов для 6-го пина GPIOC
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);                 // выключаем светодиод

    // инициализируем 3-ий светодиод

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER14_0);          // Настройка работы 6-го пина GPIOС в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT14);           // Настройка на PushPull работу 6-го пина GPIOC (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);   // Настройка скорости работы 6-го пина GPIOC на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14);          // Отключение PU/PD резисторов для 6-го пина GPIOC
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);                 // выключаем светодиод

    // инициализируем 4-ий светодиод

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER13_0);          // Настройка работы 6-го пина GPIOС в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT13);           // Настройка на PushPull работу 6-го пина GPIOC (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR13_0);   // Настройка скорости работы 6-го пина GPIOC на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR13);          // Отключение PU/PD резисторов для 6-го пина GPIOC
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13);                 // выключаем светодиод
    
    // инициализируем 5-ий светодиод

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER10_0);          // Настройка работы 6-го пина GPIOС в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT10);           // Настройка на PushPull работу 6-го пина GPIOC (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR10_0);   // Настройка скорости работы 6-го пина GPIOC на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR10);          // Отключение PU/PD резисторов для 6-го пина GPIOC
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);                 // выключаем светодиод
    
    // инициализируем 6-ий светодиод

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER4_0);          // Настройка работы 6-го пина GPIOС в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT4);           // Настройка на PushPull работу 6-го пина GPIOC (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR4_0);   // Настройка скорости работы 6-го пина GPIOC на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR4);          // Отключение PU/PD резисторов для 6-го пина GPIOC
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR4);                 // выключаем светодиод
    

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
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS4);
    else
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR4);
}

// основная задача
void main_task(volatile bool *mode_button, volatile short int *led_button)
{
    if (mode_button == 1)
}

