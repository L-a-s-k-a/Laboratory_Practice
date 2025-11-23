#include "init.h"

void Clock_Init_HSI_PLL_100MHz(void) // настраиваем тактирование на 100 МГц

{
    // 1. Включаем тактирование PWR и ставим режим питания Scale1 (подходит для 100 МГц)
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);

    // VOS — это поле из нескольких бит, очистим и выставим Scale1
    CLEAR_BIT(PWR->CR, PWR_CR_VOS);
    SET_BIT(PWR->CR, PWR_CR_VOS); // для F4 это "11", Scale1

    // 2. Настройка FLASH: число тактов ожидания (LATENCY) под 100 МГц 
    CLEAR_BIT(FLASH->ACR, FLASH_ACR_LATENCY); // очистить поле LATENCY
    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_3WS); // 3 wait states для 100 МГц

    // 3. Включаем HSI и ждём готовности
    SET_BIT(RCC->CR, RCC_CR_HSION);
    while (READ_BIT(RCC->CR, RCC_CR_HSIRDY) == 0U)  
    {
        // ждём стабилизации HSI
    }

    // 4. Убеждаемся, что SYSCLK сейчас от HSI 
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW); // сброс поля выбора источника SYSCLK
    SET_BIT(RCC->CFGR, RCC_CFGR_SW_HSI); // выбрать HSI как SYSCLK
    while ((READ_BIT(RCC->CFGR, RCC_CFGR_SWS)) != RCC_CFGR_SWS_HSI)
    {
        // ждём, пока источник SYSCLK реально не станет HSI 
    }

    // 5. Выключаем PLL перед перенастройкой
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != 0U)
    {
        // ждём, пока PLL полностью выключится 
    }

    // 6. Отключаем HSE, bypass и CSS 
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON);

    // 7. Настраиваем делители шин: AHB = /1, APB1 = /2, APB2 = /1 
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_HPRE |
                         RCC_CFGR_PPRE1 |
                         RCC_CFGR_PPRE2);

    // HPRE = /1 = 100 МГц 
    // APB1 = /2 = 50 МГц 
    // APB2 = /1 = 100 МГц 
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV2);

    /* 8. Настраиваем PLLCFGR для получения 100 МГц от HSI

       Формула:
         VCO_in  = HSI / PLLM   = 16 МГц / 8 = 2 МГц
         VCO_out = VCO_in * PLLN = 2 МГц * 200 = 400 МГц
         SYSCLK  = VCO_out / PLLP = 400 / 4 = 100 МГц
         PLLQ    = 8 → 400 / 8 = 50 МГц

       Значит:
         PLLM = 8
         PLLN = 200
         PLLP = 4
         PLLQ = 8
         PLLSRC = 0 (HSI)
    */

    // Источник PLL: PLLSRC = 0 → HSI 
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC);

    // PLLM: сначала очищаем, потом пишем 8 
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_Msk);
    SET_BIT(RCC->PLLCFGR, (8U << RCC_PLLCFGR_PLLM_Pos));

    // PLLN: очищаем поле и пишем 200 
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_Msk);
    SET_BIT(RCC->PLLCFGR, (200U << RCC_PLLCFGR_PLLN_Pos));

    // PLLP: очищаем поле и устанавливаем деление на 4 (код 01b) 
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_Msk);
    SET_BIT(RCC->PLLCFGR, (1U << 16)); /* PLLP = 4 (01b) - бит 16 = 1, бит 17 = 0 */

    // PLLQ: очищаем поле и пишем 8 
    CLEAR_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_Msk);
    SET_BIT(RCC->PLLCFGR, (8U << RCC_PLLCFGR_PLLQ_Pos));

    // 9. Включаем PLL и ждём, пока он поднимется 
    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == 0U)
    {
        // ждём готовности PLL 
    }

    // 10. Переключаем SYSCLK на PLL 
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW); // очистить поле SW
    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL); // выбрать PLL как SYSCLK

    while ((READ_BIT(RCC->CFGR, RCC_CFGR_SWS)) != RCC_CFGR_SWS_PLL)
    {
        // ждём, пока источник SYSCLK реально не станет PLL
    }

    // 11. Обновляем переменную SystemCoreClock для дальнейших расчётов
    SystemCoreClock = 100000000U;
}

void MCO_init(void) // инициализация MCO для проверки тактовой частоты
{
    // Для PC9
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // включаем тактирование для PORTC
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE9_Msk); // очищаем бит
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE9_1); // включаем режим альтернативной функции

    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT9_Msk); // очищаем бит
    CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk); // очищаем бит
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk); // ставим очень высокую скорость
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD9_Msk); // очищаем бит

    CLEAR_BIT(GPIOC->AFR[1], 0xFU << ((9U - 8U) * 4U)); // очищаем бит
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2 | RCC_CFGR_MCO2PRE); // очищаем бит
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2_0 | RCC_CFGR_MCO2_1); // ставим источник тактирования PLLCLK
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_1 | RCC_CFGR_MCO2PRE_2); // ставим предделитель
    // когда включён 1 и 2, частота равна 25МГц, следовательно предделитель равен 4.
    // Это значит что при включении 1 и 2, получается комбинация 110, это значит что нулевой бит — 27, первый — 28 и второй — 29
}

void GPIO_Init(void) // инициализация GPIO
{
    // Включаем тактирование портов
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN;
    
    // Настраиваем кнопку PC13 как вход с подтяжкой к питанию
    GPIOC->MODER &= ~GPIO_MODER_MODER13;
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR13;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_0;
    
    // Настраиваем 6 светодиодов на PA5-PA10 как выходы
    GPIOA->MODER |= GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | 
                   GPIO_MODER_MODER7_0 | GPIO_MODER_MODER8_0 | 
                   GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0;
    
    // Настраиваем EXTI для PC13
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    SYSCFG->EXTICR[3] = (SYSCFG->EXTICR[3] & ~SYSCFG_EXTICR4_EXTI13_Msk) | SYSCFG_EXTICR4_EXTI13_PC;
    
    EXTI->IMR |= EXTI_IMR_MR13;
    EXTI->RTSR |= EXTI_RTSR_TR13; // Прерывание по нарастающему фронту
    EXTI->FTSR |= EXTI_FTSR_TR13; // Прерывание по спадающему фронту
    
    NVIC_SetPriority(EXTI15_10_IRQn, 0);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void SysTick_Init(void) // Инициализация SysTick для 100 МГц
{
    SysTick->CTRL = 0; // отключаем таймер перед настройкой
    // LOAD - значение, до которого считает таймер
    SysTick->LOAD = 100000 - 1; // 100 MHz / 1000 = 100000 тиков (1 мс) прерывание каждую миллисекунду
    SysTick->VAL = 0; // обнуляем текущее значение счётчика 
    // включение таймера
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | // используем процессорную частоту
                    SysTick_CTRL_TICKINT_Msk | // разрешаем прерывания при достижении нуля
                    SysTick_CTRL_ENABLE_Msk; // запускаем таймер
}

void SysTick_Handler(void) // Обработчик SysTick
{
    systick_counter++; // увеличивается на 1 каждую миллисекунду
}

uint32_t GetTick(void) // время с начала работы программы
{
    return systick_counter;
}

void EXTI15_10_IRQHandler(void) // Обработчик прерывания EXTI
{
    if (EXTI->PR & EXTI_PR_PR13) // проверяем, вызвано ли прерывание именно нашей кнопкой
    {
        EXTI->PR = EXTI_PR_PR13; // сбрасываем флаг чтобы прерывание не вызывалось бесконечно
        
        static uint32_t last_interrupt_time = 0; // static означает что переменная сохраняет своё значение между вызовами функции
        uint32_t current_time = GetTick();
        
        // Защита от дребезга - 50 мс
        if ((current_time - last_interrupt_time) < 50)
        {
            return; // если прошло менее 50 миллисекунд с последнего прерывания то выходим из обработчика
        }
        
        if (GPIOC->IDR & GPIO_IDR_ID13) // если кнопка сейчас отпущена (нарастающий фронт)
        {
            if (button_state == 1) // если кнопка до этого была нажата
            {
                button_press_duration = current_time - button_press_start_time; // вычисляем длительность нажатия кнопки
                button_state = 0;
            }
        }
        else
        {
            // Кнопка сейчас нажата (спадающий фронт)
            button_press_start_time = current_time;
            button_state = 1;
        }
        
        last_interrupt_time = current_time; // обновляем время последнего прерывания
    }
}

void SetLED(uint8_t led_num) // Включение конкретного светодиода
{
    GPIOA->ODR &= ~(GPIO_ODR_OD5 | GPIO_ODR_OD6 | GPIO_ODR_OD7 | GPIO_ODR_OD8 | GPIO_ODR_OD9 | GPIO_ODR_OD10); // Сначала выключаем все светодиоды

    switch(led_num) // Включаем нужный светодиод
    {
        case 0: GPIOA->ODR |= GPIO_ODR_OD5; break; // первый светодиод
        case 1: GPIOA->ODR |= GPIO_ODR_OD6; break; // второй светодиод
        case 2: GPIOA->ODR |= GPIO_ODR_OD7; break; // третий светодиод
        case 3: GPIOA->ODR |= GPIO_ODR_OD8; break; // четвёртый светодиод
        case 4: GPIOA->ODR |= GPIO_ODR_OD9; break; // пятый светодиод
        case 5: GPIOA->ODR |= GPIO_ODR_OD10; break; // шестой светодиод
    }
}

void ClearAllLEDs(void) // Выключение всех светодиодов
{
    GPIOA->ODR &= ~(GPIO_ODR_OD5 | GPIO_ODR_OD6 | GPIO_ODR_OD7 |
                   GPIO_ODR_OD8 | GPIO_ODR_OD9 | GPIO_ODR_OD10);
}

void UpdateBlink(void) // Обновление мерцания светодиодов
{
    if (leds_enabled && blink_enabled) // если светодиоды включены и они мерцают
    {
        uint32_t current_time = GetTick(); // получаем текущее время
        uint32_t period = blink_periods[blink_frequency]; // получаем текущий период мерцания (0: 0.4 Гц, 1: 1.1 Гц, 2: 1.9 Гц)
        
        if ((current_time - last_blink_time) >= period) // если прошло достаточно времени с последнего переключения
        {
            // Инвертируем состояние светодиодов
            if (GPIOA->ODR & (GPIO_ODR_OD5 | GPIO_ODR_OD6 | GPIO_ODR_OD7 |
                             GPIO_ODR_OD8 | GPIO_ODR_OD9 | GPIO_ODR_OD10)) // если горит хотя бы один светодиод
            {
                ClearAllLEDs(); // гасим все
            }
            else
            {
                SetLED(current_led); // включаем нужный
            }
            last_blink_time = current_time; // обновляем время последнего переключения
        }
    }
}
