#include "init.h"

void Init_with_memory_for_led3(void) // PA11
{
    *(uint32_t*)(0x40020000UL + 0x00UL) |= 0x400000UL; // определение режима работы
    *(uint32_t*)(0x40020000UL + 0x04UL) |= 0x0000UL; // выбираем тип выхода 
    *(uint32_t*)(0x40020000UL + 0x08UL) |= 0xC00000UL; // выбираем скорость работы
    *(uint32_t*)(0x40020000UL + 0x0CUL) |= 0x0000UL; // подтягивающий резистор
}

void GPIO_Init(void)
{
    // настройка порта PC9 в качестве MCO2
    SET_BIT(GPIOC->MODER,GPIO_MODER_MODER9_1); // настраиваем пин на альтернативный режим
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9); // настраиваем пин на максимальную скорость работы
    MODIFY_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL9_Msk, 0x00UL); // выбираем тип альтернативной функции

    // настройка порта PA8 в качестве MCO1
    SET_BIT(GPIOA->MODER,GPIO_MODER_MODER8_1); // настраиваем пин на альтернативный режим
    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED8); // настраиваем пин на максимальную скорость работы
    CLEAR_BIT(GPIOA->AFR[1], GPIO_AFRH_AFSEL8); // выбираем тип альтернативной функции
}

void GPIO_Init_with_CMSIS(void) // PA5
{
    /////////////////////////////////////////////////////////////////////

    // Включаем тактирование GPIOA GPIOB и GPIOC (бит 0 в RCC->AHB1ENR)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    /////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////

    // Настройка PB1 как вход с подтяжкой вверх (кнопка)
    GPIOB->MODER &= ~(0x3 << (1 * 2));  
    GPIOB->PUPDR &= ~(0x3 << (1 * 2));  
    GPIOB->PUPDR |= (0x1 << (1 * 2));   // режим подтяжки вверх

    /////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////

    // Настройка PB2 как вход с подтяжкой вверх (кнопка)
    GPIOB->MODER &= ~(0x3 << (2 * 2));  
    GPIOB->PUPDR &= ~(0x3 << (2 * 2));  
    GPIOB->PUPDR |= (0x1 << (2 * 2));   // режим подтяжки вверх

    /////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////

    // Настройка PB3 как вход с подтяжкой вверх (кнопка)
    GPIOB->MODER &= ~(0x3 << (3 * 2));  
    GPIOB->PUPDR &= ~(0x3 << (3 * 2));  
    GPIOB->PUPDR |= (0x1 << (3 * 2));   // режим подтяжки вверх

    /////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////

    // Настройка PC10 как вход с подтяжкой вверх (кнопка)
    GPIOC->MODER &= ~(0x3 << (10 * 2));  
    GPIOC->PUPDR &= ~(0x3 << (10 * 2));  
    GPIOC->PUPDR |= (0x1 << (10 * 2));   // режим подтяжки вверх

    /////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////

    // Настраиваем PA5 как выход (MODER5[1:0] = 01)
    GPIOA->MODER &= ~(0x3 << (5 * 2)); // Сбрасываем биты MODER5
    GPIOA->MODER |=  (0x1 << (5 * 2)); // Устанавливаем режим Output

    /////////////////////////////////////////////////////////////////////
}

void GPIO_Init_with_my_macro(void) // PA10
{
    /////////////////////////////////////////////////////////////////////
    
    RCC_GPIO |= RCC_GPIOA_ON | RCC_GPIOB_ON | RCC_GPIOC_ON; // включение тактирования портов GPIOA, GPIOB и GPIOC

    /////////////////////////////////////////////////////////////////////

    // Настройка всех кнопок

    GPIOB_MODER = (GPIOB_MODER & ~(0x3UL << (1 * 2)));
    GPIOB_MODER = (GPIOB_MODER & ~(0x3UL << (2 * 2)));
    GPIOB_MODER = (GPIOB_MODER & ~(0x3UL << (3 * 2)));
    GPIOC_MODER = (GPIOC_MODER & ~(0x3UL << (10 * 2)));
    GPIOB_MODER |= GPIOB_MODE_PIN1_IN | GPIOB_MODE_PIN2_IN | GPIOB_MODE_PIN3_IN; // изначально режим входа на пинах
    GPIOC_MODER |= GPIOC_MODE_PIN10_IN; // изначально режим входа на пинах
    GPIOB_PUPDR = (GPIOB_PUPDR & ~(0x3UL << (1 * 2)));
    GPIOB_PUPDR = (GPIOB_PUPDR & ~(0x3UL << (2 * 2)));
    GPIOB_PUPDR = (GPIOB_PUPDR & ~(0x3UL << (3 * 2)));
    GPIOC_PUPDR = (GPIOC_PUPDR & ~(0x3UL << (10 * 2)));
    GPIOB_PUPDR |= GPIOB_PUPD_PIN1_PULL_UP | GPIOB_PUPD_PIN2_PULL_UP | GPIOB_PUPD_PIN3_PULL_UP;
    GPIOC_PUPDR |= GPIOC_PUPD_PIN10_PULL_UP;

    /////////////////////////////////////////////////////////////////////

    // Настройка пина для внешнего светодиода

    GPIOA_MODER |= GPIOA_MODE_PIN10_OUT;
    GPIOA_OTYPER |= GPIOA_OTYPE_PIN10_PP;
    GPIOA_OSPEEDR |= GPIOA_OSPEED_PIN10_HS;
    GPIOA_PUPDR |= GPIOA_PUPD_PIN10_NOPUPD;

    /////////////////////////////////////////////////////////////////////
}

void delay(volatile uint32_t count)
{
    for (; count > 0; count--)
    {
        // пустая итерация
    }
}

void ITR_Init(void)
{
    // Благодаря этому при нажатии и отпускании кнопки подключённой к пину PB1 будет вызываться функция обработчика прерывания
    // и будет происходить то что в ней написано
    
    SET_BIT(RCC->APB2ENR,RCC_APB2ENR_SYSCFGEN); // включение тактирования периферии SYSCFG
    MODIFY_REG(SYSCFG->EXTICR[0], SYSCFG_EXTICR1_EXTI1_Msk, SYSCFG_EXTICR1_EXTI1_PB); // настраиваем на работу с линией PB1
    SET_BIT(EXTI->IMR, EXTI_IMR_MR1); // настройка маскирования 1 линии
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR1); // включаем генерацию прерываний по нарастающему фронту сигнала на линии EXTI1. 
    // прерывание будет срабатывать при переходе сигнала с низкого уровня на высокий
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR1); // включаем генерацию прерываний по спадающему фронту сигнала на линии EXTI1
    // прерывание будет срабатывать при переходе сигнала с высокого уровня на низкий
    NVIC_SetPriority(EXTI1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0)); // задаём приоритет
    NVIC_EnableIRQ(EXTI1_IRQn); // разрешаем прерывания
}
