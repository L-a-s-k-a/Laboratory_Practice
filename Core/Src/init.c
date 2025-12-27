#include "init.h"
#include "interrupt.h" // для extern переменных (только объявления)

void GPIO_Init(void)
{   // по стандарту подрубаю тактирование портов
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

    // подключаю кнопки на вход с подтяжкой вверх
    MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER13, 0);
    MODIFY_REG(GPIOC->PUPDR, GPIO_PUPDR_PUPDR13, GPIO_PUPDR_PUPDR13_0);

    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER12, 0);
    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPDR12, GPIO_PUPDR_PUPDR12_0);

    MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER4, 0);
    MODIFY_REG(GPIOC->PUPDR, GPIO_PUPDR_PUPDR4, GPIO_PUPDR_PUPDR4_0);

    // у светодиодов прокидываю каждому выход, задаю тип выхода пина, а потом сдвигаю в нужный бит чтоб вырубить
    // PA5 первый светодиод
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER5, GPIO_MODER_MODER5_0);
    CLEAR_BIT(GPIOA->OTYPER, (1U << LED1_PIN));
    SET_BIT(GPIOA->BSRR, (1UL << (LED1_PIN + 16)));

    // PC5 второй светодиод
    MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER5, GPIO_MODER_MODER5_0);
    CLEAR_BIT(GPIOC->OTYPER, (1U << LED2_PIN));
    SET_BIT(GPIOC->BSRR, (1UL << (LED2_PIN + 16)));

    // PA6 третий светодиод
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER6, GPIO_MODER_MODER6_0);
    CLEAR_BIT(GPIOA->OTYPER, (1U << LED3_PIN));
    SET_BIT(GPIOA->BSRR, (1UL << (LED3_PIN + 16)));

    // PB1 четвёртый светодиод
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER1, GPIO_MODER_MODER1_0);
    CLEAR_BIT(GPIOB->OTYPER, (1U << LED4_PIN));
    SET_BIT(GPIOB->BSRR, (1UL << (LED4_PIN + 16)));

    // PC6 пятый светодиод
    MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER6, GPIO_MODER_MODER6_0);
    CLEAR_BIT(GPIOC->OTYPER, (1U << LED5_PIN));
    SET_BIT(GPIOC->BSRR, (1UL << (LED5_PIN + 16)));

    // PB2 шестой светодиод
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER2, GPIO_MODER_MODER2_0);
    CLEAR_BIT(GPIOB->OTYPER, (1U << LED6_PIN));
    SET_BIT(GPIOB->BSRR, (1UL << (LED6_PIN + 16)));
}

void RCC_Init(void)
{
    // Код с использованием делителя
    /*MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80UL); 
    CLEAR_REG(RCC->CFGR); 
    while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON); 
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON); 
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET); 
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP); 
 
    // Настройка главного регистра RCC 
    SET_BIT(RCC->CR, RCC_CR_HSION);                              
    while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == RESET);
 
    //Настройка регистра PLLCFGR
    CLEAR_REG(RCC->PLLCFGR);                                        // Очищаем регистр PLLCFGR
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSI);                  // Источник тактирования HSI
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_3);                      // Деление источника тактирования на 8
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_5); // Настрока умножения на 96
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0 | RCC_PLLCFGR_PLLP_1); // Делитель для выхода PLL(PLLCLK) на 2 (6)
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0 );                     // Делитель для выхода PLL(PLLCLK) на 2 (12)
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_1);                      // Делитель для выхода PLL(PLLCLK) на 2 (8)
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2);                      // Делитель для USB на 15

    //Настройка регистра CFGR
    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);                            // Источник системного тактирования -> выход PLL 96мгц
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV1);                       // Делитель AHB1 -> 1
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV2);                        // Делитель APB1 -> 2 (48 МГц) 24?
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE2_DIV1);                        // Делитель APB2 -> 1 (96 МГц) 48?
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);  
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2_0 | RCC_CFGR_MCO2_1);          // Выход MCO2 -> Sysclk
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_1);    // Делитель для MCO2 -> 4

    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_3WS);
    SET_BIT(RCC->CR, RCC_CR_PLLON);

    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);*/

    // Включаю HSI и делители (по примеру)
    SET_BIT(RCC->CR, RCC_CR_HSION);         //подруб HSI
    while(!(READ_BIT(RCC->CR, RCC_CR_HSIRDY)));     // ожидание готовности

    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_0WS);      //задержку флэш в ноль
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN);

    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_HSI);        //задаю HSI как системную частоту
    while((READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)); //ожидание переключения

    MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);       //AHB  без деления
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2);     // тут APB1/2
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1);     // тут APB2/1
}

void SysTick_Init(void)
{
    // SysTick в 1 мс при 16 Мегагерц
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);      //выключаю SysTick
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);     //Задаю тактирование от CPU
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);       // Включаю прерывание
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk, (16000 - 1) << SysTick_LOAD_RELOAD_Pos);     // 1 мс
    SysTick->VAL = 0;           //сброс счётчика в ноль
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);     // Врубаю систик
}

void Interrupt_Init(void)
{
    // EXTI: настроим только falling (нажатие)
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    // PC13 -> EXTI13 (USER) - falling only
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, SYSCFG_EXTICR4_EXTI13_PC);
    SET_BIT(EXTI->IMR, EXTI_IMR_IM13);
    CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_TR13);
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR13);
    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
    NVIC_EnableIRQ(EXTI15_10_IRQn);

    // PB12 -> EXTI12 (BTN_FREQ) - falling only (same IRQ EXTI15_10)
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI12_Msk, SYSCFG_EXTICR4_EXTI12_PB);
    SET_BIT(EXTI->IMR, EXTI_IMR_IM12);
    CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_TR12);
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR12);

    // PC4 -> EXTI4 (BTN_SEL) - falling only
    MODIFY_REG(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI4_Msk, SYSCFG_EXTICR2_EXTI4_PC);
    SET_BIT(EXTI->IMR, EXTI_IMR_IM4);
    CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_TR4);
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR4);
    NVIC_SetPriority(EXTI4_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
    NVIC_EnableIRQ(EXTI4_IRQn);
}

// функция выруба всех светодиодов (сбрасывает в 0)
void leds_all_off(void)
{
    SET_BIT(GPIOA->BSRR, (1UL << (LED1_PIN + 16)) | (1UL << (LED3_PIN + 16)));
    SET_BIT(GPIOB->BSRR, (1UL << (LED4_PIN + 16)) | (1UL << (LED6_PIN + 16)));
    SET_BIT(GPIOC->BSRR, (1UL << (LED2_PIN + 16)) | (1UL << (LED5_PIN + 16)));
}

// выбор для периода для двух разных режимов
uint32_t Mode0_PeriodFromIndex(uint8_t idx)
{
    switch(idx)
    {
        case 0: return MODE0_FREQ0_MS;
        case 1: return MODE0_FREQ1_MS;
        case 2: return MODE0_FREQ2_MS;
        default: return MODE0_FREQ0_MS;
    }
}
uint32_t Mode1_PeriodFromIndex(uint8_t idx)
{
    switch(idx)
    {
        case 0: return MODE1_FREQ0_MS;
        case 1: return MODE1_FREQ1_MS;
        case 2: return MODE1_FREQ2_MS;
        default: return MODE1_FREQ0_MS;
    }
}

// обновление для режима 0 (для пар)
void Update_Mode0(void)
{
    uint8_t freq_idx = mode0_group_freq[mode0_pair_index];  //задаётся частота для текущей пары
    uint32_t P = Mode0_PeriodFromIndex(freq_idx);   //период пары
    if(P < 2) P = 2;            //время минимума (2мс)
    uint32_t half = P / 2;      //половина периода

    leds_all_off();   //гашу светодиоды

    if(mode0_pair_tick < half)      //первая половина периода
    {
        if(mode0_pair_index == 0)       // Пара 3–4
        {
            SET_BIT(GPIOA->BSRR, (1UL << LED3_PIN));
            SET_BIT(GPIOB->BSRR, (1UL << LED4_PIN));
        }
        else if(mode0_pair_index == 1)      // Пара 2–5
        {
            SET_BIT(GPIOC->BSRR, (1UL << LED2_PIN));
            SET_BIT(GPIOC->BSRR, (1UL << LED5_PIN));
        }
        else        //пара 1–6
        {
            SET_BIT(GPIOA->BSRR, (1UL << LED1_PIN));
            SET_BIT(GPIOB->BSRR, (1UL << LED6_PIN));
        }
    }

    if(mode0_pair_tick >= P)    //конец периода
    {
        mode0_pair_tick = 0;        //сбрасываю таймер
        mode0_pair_index++;         //следующая пара
        if(mode0_pair_index >= 3) mode0_pair_index = 0;     //зациклиавние по кругу
    }
}

// Обновление режима 1 (по одному, после staging мерцание)
void Update_Mode1(void)
{
    if(staging_index < 6)       //пока не все светодиоды включены
    {
        if(staging_tick >= STAGING_INTERVAL_MS)// окончание задержки
        {
            mode1_led_active[staging_index] = true;     //активация светодиодов
            mode1_led_ticks[staging_index] = 0;         // сброс таймера
            staging_index++;                    // слежующий светодиод
            staging_tick = 0;                  // сброс задержки
        }
    }

    leds_all_off();     //опять гашу все светодиоды

    for(uint8_t i = 0; i < 6; ++i)      //перебор светодиодов
    {
        if(!mode1_led_active[i]) continue;  //прорускаю неактивные

        uint8_t idx = mode1_led_freq[i];        //индекс частоты
        uint32_t P = Mode1_PeriodFromIndex(idx);        //период
        if(P < 2) P = 2;        //минимум
        uint32_t half = P / 2;  //половина периода

        if(mode1_led_ticks[i] < half)       //первая половина
        {
            switch(i)   //включение нужного диода
            {
                case 0: SET_BIT(GPIOA->BSRR, (1UL << LED1_PIN)); break;
                case 1: SET_BIT(GPIOC->BSRR, (1UL << LED2_PIN)); break;
                case 2: SET_BIT(GPIOA->BSRR, (1UL << LED3_PIN)); break;
                case 3: SET_BIT(GPIOB->BSRR, (1UL << LED4_PIN)); break;
                case 4: SET_BIT(GPIOC->BSRR, (1UL << LED5_PIN)); break;
                case 5: SET_BIT(GPIOB->BSRR, (1UL << LED6_PIN)); break;
            }
        }

        if(mode1_led_ticks[i] >= P) mode1_led_ticks[i] = 0; //перезапуск периода
    }
}
