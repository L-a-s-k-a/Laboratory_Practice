#include "init_lab2.h"

void GPIO_Init_Led_and_button(void)
{

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); // Включение тактирования GPIOB и GPIOC

    /*-----------------------------------------Настройка светодиодов-----------------------------------------*/
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);              // Настройка пина PB0 на выход, регистр GPIOx_MODER
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);             // Установление PB0 в режим pull-push, регистр OTYPER
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);      // Устанавливаем скорость бита PB0 (средняя), регистр OSPEEDR
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD0_0);            // Отключаем подтягивающий резистор PB0, регистр PUPDR
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);                    // Установление на пине PB0 0, регистр GPIOx_BSRR

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);              // Настройка пина PB7 на выход, регистр GPIOx_MODER
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);             // Установление PB7 в режим pull-push, регистр OTYPER
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);      // Устанавливаем скорость бита PB7 (средняя), регистр OSPEEDR
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD7_0);            // Отключаем подтягивающий резистор PB7, регистр PUPDR
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);                    // Установление на пине PB7 0, регистр GPIOx_BSRR

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);             // Настройка пина PB14 на выход, регистр GPIOx_MODER
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);            // Установление PB14 в режим pull-push, регистр OTYPER
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);     // Устанавливаем скорость бита PB14 (средняя), регистр OSPEEDR
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD14_0);           // Отключаем подтягивающий резистор PB14, регистр PUPDR
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);                   // Установление на пине PB14 0, регистр GPIOx_BSRR

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE8_0);              // Настройка пина PB8 на выход, регистр GPIOx_MODER
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_8);             // Установление PB8 в режим pull-push, регистр OTYPER
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);      // Устанавливаем скорость бита PB8 (средняя), регистр OSPEEDR
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD8_0);            // Отключаем подтягивающий резистор PB8, регистр PUPDR
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);                    // Установление на пине PB8 0, регистр GPIOx_BSRR

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE9_0);              // Настройка пина PB9 на выход, регистр GPIOx_MODER
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_9);             // Установление PB9 в режим pull-push, регистр OTYPER
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR9_0);      // Устанавливаем скорость бита PB9 (средняя), регистр OSPEEDR
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD9_0);            // Отключаем подтягивающий резистор PB9, регистр PUPDR
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR9);                    // Установление на пине PB9 0, регистр GPIOx_BSRR

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE15_0);             // Настройка пина PB15 на выход, регистр GPIOx_MODER
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_15);            // Установление PB15 в режим pull-push, регистр OTYPER
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR15_0);     // Устанавливаем скорость бита PB15 (средняя), регистр OSPEEDR
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD15_0);           // Отключаем подтягивающий резистор PB15, регистр PUPDR
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);                   // Установление на пине PB15 0, регистр GPIOx_BSRR
}

void RCC_init_clocking(void) // Настройка тактирования
{
    /*----------------Произведем очистку регистров RCC, с целью не попадания лишней информации----------------*/
    MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);                  // Настройка внутренней высокоскоростной синхронизации тактовых импульсов, для возможности корректировки частоты (регистр RCC_CR)
    CLEAR_REG(RCC->CFGR);                                        // Очистка регистра RCC_CFGR
    while (READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET);          // Проверка очистки RCC_CFGR, если очистка была, то продолжаем, если нет то цикл бесконечно длится
    CLEAR_BIT(RCC->CR, RCC_CR_PLLON);                            // Очистка бита PLLON (регистр RCC_CR)
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET);           // Проверка был ли очищен PLLON
    CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);             // Очистка бита CSSON и HSEON (регистр RCC_CR)
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET);           // Проверка был ли очищен HSERDY
    CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);                           // Очистка бита HSEBYP (регистр RCC_CR)

    /*---------------------------------------Настройка регистра RCC_CR----------------------------------------*/
    SET_BIT(RCC->CR, RCC_CR_HSEON);                              // Запускаем внешний кварцевый резонатор (регистр RCC_CR, бит HSEON)
    while (READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET);           // Ждём пока запустится внешний резонатор
    SET_BIT(RCC->CR, RCC_CR_CSSON);                              // Запускаем систему безопасности тактирования (регистр RCC_CR, бит CSSON)
    SET_BIT(RCC->CR, RCC_CR_HSEBYP);                             // Использование внешнего резонатора (регистр RCC_CR, бит HSEBYP)
    CLEAR_BIT(RCC->CR, RCC_CR_HSION);                            // Выключаем внутренний RC-генератор (HSI), так как используем HSE

    /*----------------------------------------Настройка регистров PLL-----------------------------------------*/
    CLEAR_REG(RCC->PLLCFGR);                                                                                  // Предварительная очистка (регистр PLLCFGR)
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE);                                                            // В качестве источника тактирования для PLL выбираем HSE (регистр PLLCFGR) (выдает частоту 8МГц)                                         // Включение тактирования HSE
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLM_2);                                                                // Установим делитель /M на 4 (регистр PLLCFGR) (теперь частота шины 2 МГц)
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLN_3 | RCC_PLLCFGR_PLLN_5 | RCC_PLLCFGR_PLLN_6 | RCC_PLLCFGR_PLLN_8); // Настройка множителя xN на 360 (регистр PLLCFGR) (теперь частота 720 МГц)
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLP_0);                                                                // Установим делитель /P на 4 (регистр PLLCFGR) (теперь частота шины 180 МГц)
    SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_0 | RCC_PLLCFGR_PLLQ_1 | RCC_PLLCFGR_PLLQ_2 | RCC_PLLCFGR_PLLQ_3); // Делитель для USB /Q установим 15 (регистр PLLCFGR) (частота на шине USB 48 МГц)

    SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);                         // Источник системного тактирования -> выход PLL
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_HPRE_DIV1);                    // Делитель AHB1 устанавливаем 1
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE1_DIV4);                     // Делитель APB1 устанавливаем 4 (45 МГц)
    SET_BIT(RCC->CFGR, RCC_CFGR_PPRE2_DIV2);                     // Делитель APB2 устанавливаем 2 (90 МГц)
    CLEAR_BIT(RCC->CFGR, RCC_CFGR_MCO2);                         // Выход  MCO2 для Sysclk
    SET_BIT(RCC->CFGR, RCC_CFGR_MCO2PRE_1 | RCC_CFGR_MCO2PRE_2); // Делитель для MCO2 устанавливаем 4

    /*--------------------------------Устанавливаем задержку для памяти (Flash)-----------------------------------*/
    SET_BIT(FLASH->ACR, FLASH_ACR_LATENCY_5WS);                  // Задержка памяти FLASH = 5 тактов (для 180 МГц)

    SET_BIT(RCC->CR, RCC_CR_PLLON);                              // Включение PLL
    while (READ_BIT(RCC->CR, RCC_CR_PLLRDY) == RESET);           // Ожидание включения PLL
}

void ITR_init(void)                                                                             // Настройка прерывания кнопки
{
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);                                                // Включение тактирования SYSCFG - необходим для настройки EXTI

    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI13_Msk, SYSCFG_EXTICR4_EXTI13_PC);         // Настройка EXTI13 на PC13 (EXTICR[3] соответствует EXTI12-15, выбираем порт C для линии 13) (кнопка №2)
    MODIFY_REG(SYSCFG->EXTICR[3], SYSCFG_EXTICR4_EXTI12_Msk, SYSCFG_EXTICR4_EXTI12_PC);         // Настройка EXTI12 на PC12 (EXTICR[3] соответствует EXTI12-15, выбираем порт C для линии 12) (кнопка №1)

    SET_BIT(EXTI->IMR, EXTI_IMR_MR12 | EXTI_IMR_MR13);                                          // Разрешает прерывания по линиям PC12 и PC13
    SET_BIT(EXTI->RTSR, EXTI_RTSR_TR12 | EXTI_RTSR_TR13);                                       // Настраиваем триггер по нарастанию фронта для кнопок (переход от 0 к 1)
    SET_BIT(EXTI->FTSR, EXTI_FTSR_TR12 | EXTI_FTSR_TR13);                                       // Настраиваем триггер по спадающему фронту для кнопок (переход от 1 к 0)

    NVIC_SetPriority(EXTI15_10_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));    // Устанавливаем приоритет прерывания (высший)
    NVIC_EnableIRQ(EXTI15_10_IRQn);                                                             // Включаем прерывания в контроллере NVIC (теперь процессор будет их обрабатывать)
}

void SysTick_Init(void)                                                                         // Настройка системного таймера (SysTick) для генерации прерываний каждую 1 мс
{
    CLEAR_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                          // Выключение таймера перед настройкой (безопасность)
    SET_BIT(SysTick->CTRL, SysTick_CTRL_TICKINT_Msk);                                           // Разрешение прерываний счетчика при достижении нуля
    SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);                                         // Устанавливаем источник тактирования AHB (180 МГц) без деления
    MODIFY_REG(SysTick->LOAD, SysTick_LOAD_RELOAD_Msk,
               179999 << SysTick_LOAD_RELOAD_Pos);                                              // Настраиваем счетчик что бы он считал с 179999, что соотвествует частоте 1 кГц (1 мс)
    MODIFY_REG(SysTick->VAL, SysTick_VAL_CURRENT_Msk,
               179999 << SysTick_VAL_CURRENT_Pos);                                              // Сброс счетчика в начальное состояние
    SET_BIT(SysTick->CTRL, SysTick_CTRL_ENABLE_Msk);                                            // Включение SysTick таймера
}
