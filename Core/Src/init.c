#include <init.h>

volatile uint32_t global_tick_count = 0;
volatile uint8_t btn_mode = 0;
volatile uint8_t btn_freq = 0;
volatile uint8_t btn_select = 0;

// Светодиоды PA5, PA6, PA7, PA9, PA8, PA10 - выходы
// Кнопки PB8, PB9, PB6 - входы с подтяжкой к питанию (нажата = 0)
void __GPIO_Config(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN );  // Включаем тактирование портов A и B

    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 |
                          GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0);  // Светодиоды - выход

    SET_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR9_0);   // Подтяжка к питанию на кнопки
}

// Тактирование: HSI 16 МГц -> PLL (/8 *180 /2) -> SYSCLK 180 МГц
// AHB = 180 МГц, APB1 = 45 МГц, APB2 = 90 МГц
void __RCC_Config(void)
{
    SET_BIT(RCC->CR, RCC_CR_HSION);                                 // Включаем HSI
    while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET);              // Ждём готовности

    // PLL: источник HSI, M = 8 (16/8 = 2 МГц), N = 180 (360 МГц), P = 2 (180 МГц)
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);                      // Выключаем PLL на время настройки
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);
    CLEAR_REG(RCC->PLLCFGR);
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_3);                                                                   // M = 8
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_7);    // N = 180
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0 | RCC_PLLCFGR_PLLP_1);                                            // P = 2
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_3);                                                                   // Q = 8 (45 МГц)
    SET_BIT(RCC->CR, RCC_CR_PLLON);                                 // Запускаем PLL
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);              // Ждём готовности PLL

    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);            // SYSCLK = PLL
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Ждём переключения

    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);       // AHB без делителя
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);     // APB1 делим на 4
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);     // APB2 делим на 2

    // Задержка Flash 5WS (6 тактов CPU) + prefetch и кэши
    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);
}

// Внешние прерывания: линии EXTI8, EXTI9, EXTI6 от порта B (вектор EXTI9_5)
void __ITR_Config(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);                                    // Тактирование SYSCFG

    SET_BIT(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI6_PB);                            // EXTI6 -> PB6
    SET_BIT(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI8_PB | SYSCFG_EXTICR3_EXTI9_PB);  // EXTI8 -> PB8, EXTI9 -> PB9

    SET_BIT(EXTI->IMR, EXTI_IMR_IM6 | EXTI_IMR_IM8 | EXTI_IMR_IM9);                 // Разрешаем прерывания линий
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR6 | EXTI_FTSR_TR8 | EXTI_FTSR_TR9);             // По спадающему фронту (нажатие = 0)

    NVIC_SetPriority(EXTI9_5_IRQn, 1);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

// SysTick: 180 МГц / 180000 = 1 кГц, прерывание каждую миллисекунду
void SysTick_Init_Config(void)
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);               // Выключение счётчика
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);                // Разрешаем прерывание
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);              // Источник тактирования, будет идти из AHB без деления
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, (180000 - 1) << SysTick_LOAD_RELOAD_Pos);   //Значение с которого начинается счёт
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk, (180000 - 1) << SysTick_VAL_CURRENT_Pos);    //Очистка поля
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);          // Включение счётчика
    NVIC_SetPriority(SysTick_IRQn, 0);                        // Приоритет выше, чем у кнопок
} 
