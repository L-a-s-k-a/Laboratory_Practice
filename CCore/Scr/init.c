#include "../Inc/init.h"

void GPIO_Ini(void)
{
    // Прямое обращение к памяти
    *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x02UL;      // Включение тактирование на шину GPIOB
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x0004000UL; // Настройка работы 7 пина в режиме вывода сигнала
    *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x00UL;      // Настройка на Push-Pull работу 7 пина GPIOB (Output Push-Pull)
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x0004000UL; // Настройка скорости работы 7 пина GPIOB на среднюю
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x00UL;      // Отключение PU/PD резисторов для 7 пина GPIOB

    // Обращение через дериктивы и макросы, написанные самим
    SET_BIT(RCC_GPIO_EN, RCC_GPIOA_EN);            // Включение тактирование на шину GPIOA
    SET_BIT(GPIOA_MODER, GPIOA_MODE_PIN0_OUT);     // Настройка работы 0 пина в режиме вывода сигнала
    SET_BIT(GPIOA_OTYPER, GPIOA_OTYPE_PIN0_PP);    // Настройка на Push-Pull работу 0 пина GPIOA (Output Push-Pull)
    SET_BIT(GPIOA_OSPEEDR, GPIOA_OSPEED_PIN0_MID); // Настройка скорости работы 0 пина GPIOA на среднюю
    SET_BIT(GPIOA_PUPDR, GPIOA_PUPDR_PIN0_NUPUPD); // Отключение PU/PD резисторов для 0 пина GPIOA

    // Использование библиотеки CMSIS
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);        // Включение тактирование на шину GPIOD
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE2_0);         // Настройка работы 2 пина в режиме вывода сигнала
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT_2);        // Настройка на Push-Pull работу 2 пина GPIOD (Output Push-Pull)
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR2_0); // Настройка скорости работы 2 пина GPIOD на среднюю
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPDR2_0);      // Отключение PU/PD резисторов для 2 пина GPIOD

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);  // Включение тактирование на шину GPIOC
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR12_0); // Включение внутренней подтяжки
}

uint8_t click_counter(uint8_t control, uint8_t click)
{
    control++;
    if (control < 0 || control > click)
    {
        control = 0;
    }
    return control;
}

void turning_on_the_LED(uint8_t LED)
{
    // Включение/выключение светодиодов
    if (LED == 0)
    {
        *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x800000UL;
        SET_BIT(GPIOA_BSRR, GPIOA_BSSR_PIN0_RESET);
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);
    }
    else if (LED == 1)
    {
        *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x80UL;
    }
    else if (LED == 2)
    {
        SET_BIT(GPIOA_BSRR, GPIOA_BSSR_PIN0_SET);
    }
    else if (LED)
    {
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
    }
}

void LED_flickering(uint8_t LED)
{
    // Включение/выключение свтодиодов с мерцанием
    if (LED == 0)
    {
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR0);
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);
    }
    else if (LED == 1)
    {
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
        for (uint32_t timer = 0; timer < 150000; timer++)
        {
        }
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
        for (uint32_t timer = 0; timer < 150000; timer++)
        {
        }
    }
    else if (LED == 2)
    {
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
        for (uint32_t timer = 0; timer < 150000; timer++)
        {
        }
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS0);
        for (uint32_t timer = 0; timer < 150000; timer++)
        {
        }
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR0);
    }
    else if (LED == 3)
    {
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
        for (uint32_t timer = 0; timer < 75000; timer++)
        {
        }
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
        for (uint32_t timer = 0; timer < 75000; timer++)
        {
        }
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS0);
        for (uint32_t timer = 0; timer < 75000; timer++)
        {
        }
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);
        for (uint32_t timer = 0; timer < 75000; timer++)
        {
        }
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR0);
    }
}

void change_PC12(uint8_t LED_pc13)
{
    // Изменение параметров PC12 с входа на вывод
    if (LED_pc13 == 1)
    {
        CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR12_0);      // Отключение внутренней подтяжки
        SET_BIT(GPIOC->MODER, GPIO_MODER_MODE12_0);         // Настройка 12 пина на выход GPIOC
        SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR12_0); // Настройка скорости работы 12 пина GPIOC на среднюю
        SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);               // Включение светодиода на 12 пине GPIOC
    }

    // Изменение параметров PC12 с вывода на вход
    else if (LED_pc13 == 0)
    {
        SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR12_0);          // Включение внутренней подтяжки
        CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE12_0);         // Включение работы 12 пина GPIOC на вход
        CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR12_0); // Включение скорости работы 12 пина GPIOC на низкую
        SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12);                 // Отключение светодидо на 12 пине GPIOC
    }
}