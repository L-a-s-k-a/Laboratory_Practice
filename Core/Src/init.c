#include "../Inc/init.h"
#include "init.h"

#include "stm32f4xx.h"

void Clock_Init_HSE_PLL_168MHz(void)
{
    //Включаем тактирование блока питания PWR и ставим Scale 1(для частот до 168 МГц) 

    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    SET_BIT(PWR->CR, PWR_CR_VOS);  

    //Включаем HSE и ждём стабилизации 

    SET_BIT(RCC->CR, RCC_CR_HSEON);
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == 0U)
    {
        //ждём, пока HSE не стабилизируется
    }


    MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS); 

    /*Настраиваем делители шин: AHB, APB1, APB2

       - AHB (HCLK)  = SYSCLK / 1  = 168 МГц  (максимум)
       - APB1 (PCLK1)= HCLK  / 4   = 42  МГц  (максимум для APB1)
       - APB2 (PCLK2)= HCLK  / 2   = 84  МГц  (максимум для APB2)
    */

    MODIFY_REG(RCC->CFGR,
               RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2,
               RCC_CFGR_HPRE_DIV1 |                                  // AHB = /1
                   RCC_CFGR_PPRE1_DIV4 |                                // APB1 = /4
                   RCC_CFGR_PPRE2_DIV2);                            // APB2 = /2

    /* Настраиваем PLL под 168 МГц от HSE = 8 МГц:
           PLLM = 8, PLLN = 336, PLLP = 2, PLLQ = 7 */

    WRITE_REG(RCC->PLLCFGR,
              (8U << RCC_PLLCFGR_PLLM_Pos) |      // PLLM = 8
                  (336 << RCC_PLLCFGR_PLLN_Pos) | // PLLN = 336
                  (0U << RCC_PLLCFGR_PLLP_Pos) |  // PLLP = 2 
                  RCC_PLLCFGR_PLLSRC_HSE |        // источник PLL = HSE
                  (7U << RCC_PLLCFGR_PLLQ_Pos));  // PLLQ = 7 (~48 МГц)

    //Включаем PLL и ждём готовности

    SET_BIT(RCC->CR, RCC_CR_PLLON);
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == 0U)
    {
        //ждём, пока PLL не поднимется
    }

    //Переключаем системное тактирование на PLL

    MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    {
        //ждём, пока в статусе не появится "PLL как SYSCLK"
    }

    CLEAR_BIT(RCC->CR, RCC_CR_HSION);

    SystemCoreClock = 168000000U;
}

void SysTick_Init_1ms(void)
{
    //Останавливаем SysTick на время настройки 
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);

    // Счётчик перезагрузки:частота / 1000 - 1  → период 1 мс
    uint32_t reload = 168000000U / 1000U - 1U;

    WRITE_REG(SysTick->LOAD, reload); // значение перезагрузки
    WRITE_REG(SysTick->VAL, 0U);      // сбрасываем текущий счётчик

    // Источник — системный такт (SYSCLK), включаем прерывания и сам таймер 
    CLEAR_BIT(SysTick->CTRL,
              SysTick_CTRL_CLKSOURCE_Msk |
                  SysTick_CTRL_TICKINT_Msk |
                  SysTick_CTRL_ENABLE_Msk);

    SET_BIT(SysTick->CTRL,
            SysTick_CTRL_CLKSOURCE_Msk |   // тактировать от ядра (SYSCLK)
                SysTick_CTRL_TICKINT_Msk | // разрешить прерывания
                SysTick_CTRL_ENABLE_Msk);  // включить счётчик
}

// PA0, PA5,PA6 срабатывание по спаду 1->0

void Buttons_GPIO_Init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE0_Msk);
    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE5_Msk);
    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE6_Msk);

    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT0_Msk);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT5_Msk);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT6_Msk);

    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD0_0);
    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD5_0);
    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD6_0);
}

// Настройка прерываний кнопки: назначение EXTI на линии, прерывание по спаду 1->0. Приоритет у PA0 выше чем PA5

void Buttons_EXTI_Init(void)
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

    // PA0 -> EXTI0
    CLEAR_BIT(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI0);

    // PA5 -> EXTI5 
    CLEAR_BIT(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI5);

    // PA6 -> EXTI6 
    CLEAR_BIT(SYSCFG->EXTICR[1], SYSCFG_EXTICR2_EXTI6);

    // Триггеры
    // Кнопка 1 — только спад 
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR0);
    CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_TR0);

    // Кнопка 2 — оба фронта 
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR5);
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR5);

    // Кнопка 3 — только спад 
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR6);
    CLEAR_BIT(EXTI->RTSR, EXTI_RTSR_TR6);

    // Разрешить 
    SET_BIT(EXTI->IMR, EXTI_IMR_MR0 | EXTI_IMR_MR5 | EXTI_IMR_MR6);

    // Очистить флаги 
    WRITE_REG(EXTI->PR, EXTI_PR_PR0 | EXTI_PR_PR5 | EXTI_PR_PR6);

    // Приоритеты 
    NVIC_SetPriority(EXTI0_IRQn, 5);
    NVIC_EnableIRQ(EXTI0_IRQn);

    NVIC_SetPriority(EXTI9_5_IRQn, 6);
    NVIC_EnableIRQ(EXTI9_5_IRQn);
}
// Настройка GPIO портов. PD1, PD2, PD3, PD4, PD6, PD7

void LEDs_GPIO_Init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);

    // PD1
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE1_0);         
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT1_Msk);     
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR1_1); 
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD1_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR1);

    // PD2
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE2_0);        
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT2_Msk);    
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR2_1); 
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD2_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);

    // PD3
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE3_0);         
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT3_Msk);     
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR3_1); 
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD3_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR3);

    //PD4
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE4_0);         
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT4_Msk);    
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR4_1); 
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD4_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR4);

    //PD6
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE6_0);         
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT6_Msk);     
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR6_1); 
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD6_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR6);

    //PD7
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE7_0);         
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT7_Msk);     
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_1); 
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD7_Msk);
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);
}

// Выключить все 6 светодиодов
void LED_AllOff(void)
{
    SET_BIT(GPIOD->BSRR,
            GPIO_BSRR_BR1 |
                GPIO_BSRR_BR2 |
                GPIO_BSRR_BR3 |
                GPIO_BSRR_BR4 |
                GPIO_BSRR_BR6 |
                GPIO_BSRR_BR7);
}

// Включить нужный светодиод (остальные остаются как были)
void LED_On_Index(uint8_t index)
{
    switch (index)
    {
    case 0:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS1);
        break;
    case 1:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
        break;
    case 2:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS3);
        break;
    case 3:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS4);
        break;
    case 4:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6);
        break;
    case 5:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS7);
        break;
    default:
        break;
    }
}
// Выключить конкретный светодиод по индексу
void LED_Off_Index(uint8_t index)
{
    switch (index)
    {
    case 0:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR1);
        break;
    case 1:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);
        break;
    case 2:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR3);
        break;
    case 3:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR4);
        break;
    case 4:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR6);
        break;
    case 5:
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);
        break;
    default:
        break;
    }
}

// Включаем только один активный светодиод, остальные гасим
void LED_SetActive(uint8_t index)
{
    LED_AllOff();
    LED_On_Index(index);
}

void LED_Toggle(uint8_t index)
{
    uint16_t pin;
    switch (index)
    {
        case 0: pin = 1; break;
        case 1: pin = 2; break;
        case 2: pin = 3; break;
        case 3: pin = 4; break;
        case 4: pin = 6; break;
        case 5: pin = 7; break;
        default: return;
    }

    if (GPIOD->ODR & (1U << pin))
        GPIOD->BSRR = (1U << (pin + 16));
    else
        GPIOD->BSRR = (1U << pin);
}

