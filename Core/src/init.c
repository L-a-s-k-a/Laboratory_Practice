#include "init.h"

#define HCLK_HZ 8000000U
#define BUTTON_LINES (EXTI_IMR_MR0 | EXTI_IMR_MR1 | EXTI_IMR_MR2)

void Board_Init(void)
{
    
    RCC->CR &= ~RCC_CR_HSEBYP; // использовать HSE
    RCC->CR |= RCC_CR_HSEON; // запуск HSE
    while ((RCC->CR & RCC_CR_HSERDY) == 0U) { //проверка флага и ожидание время на включение
    }


    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2); //  нули в полях делителей
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSE; // переключаемся на HSE
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) { // ожидание включения
    }

    const uint32_t gpio_clocks = RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIODEN; // маска для вкл тактирования гпио
    const uint32_t led_fields = (0xFFU << 24U) | (0xFU << 16U); // очистка полей диодов тк в moder для каждого по 2 бита
    const uint32_t led_output = (0x55U << 24U) | (0x5U << 16U); // маска для настройки на вход диодов

    RCC->AHB1ENR |= gpio_clocks; // включаем тактирование
    while ((RCC->AHB1ENR & gpio_clocks) != gpio_clocks) { // ждем
    }

   
    GPIOD->BSRR = LED_PINS << 16U; // заранее выключенное состояние
    GPIOD->OTYPER &= ~LED_PINS; //push pull
    GPIOD->PUPDR &= ~led_fields; // no подтяжка
    GPIOD->MODER = (GPIOD->MODER & ~led_fields) | led_output; // настройка на выходы 


    GPIOA->MODER &= ~0x3FU; // очистка
    GPIOA->PUPDR = (GPIOA->PUPDR & ~0x3FU) | (2U << 2U) | (2U << 4U); // подтяжка к земле кроме встроенной кнопки

    if (SysTick_Config(HCLK_HZ / 1000U) != 0U) { // тики на одну мс и ждем включения
        for (;;) {
        }
    }

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // тактирование
    while ((RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN) == 0U) {
    }

    EXTI->IMR &= ~BUTTON_LINES; // не выполнять прерывания во время настройки
    SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0 |
                           SYSCFG_EXTICR1_EXTI1 | SYSCFG_EXTICR1_EXTI2); // обнуляем поля выбора порта для линий 0 1 2
    EXTI->FTSR |= BUTTON_LINES; // реакция на спад 1-0 для фильтра дребезга
    EXTI->RTSR |= BUTTON_LINES; // реакция на фронт 
    EXTI->PR = BUTTON_LINES; // очистка флагов

    NVIC_ClearPendingIRQ(EXTI0_IRQn); // очистка ожидающих запросов
    NVIC_ClearPendingIRQ(EXTI1_IRQn); //
    NVIC_ClearPendingIRQ(EXTI2_IRQn);// 
    EXTI->IMR |= BUTTON_LINES;// расрешение передачи запросов от линий exti
    NVIC_EnableIRQ(EXTI0_IRQn); // расрешение обсудживать прерывания кнопок
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);
}
