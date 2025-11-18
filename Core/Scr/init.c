#include "../Inc/init.h"

uint8_t first_led = 0;
uint8_t second_led = 0;
uint8_t third_led = 0;
uint8_t fourth_led = 0;

void GPIO_INIT(void)
{
    // Обращение через дерективы и собственноручно написанные макросы
    SET_BIT(RCC_AHB1EN, RCC_GPIOAEN_EN);                // включение тактирования шины GPIOA
    SET_BIT(RCC_GPIOA_MODER, RCC_GPIOA_P4_OUT);         // настройка режима работы порта A4 на вывод сигнала
    SET_BIT(RCC_GPIOA_OTYPER, RCC_GPIOA_OTYPER_P4_PP);  // настройка типа выхода порта A4 на режим Output Push-Pull
    SET_BIT(RCC_GPIOA_OSPEED, RCC_GPIOA_OSPEED_P4_MED); // настройка скорости работы порта A4 на среднюю
    SET_BIT(RCC_GPIOA_PUPDR, RCC_GPIOA_PUPDR_P4_RST);   // настройка подтяжек/стяжек пина A4 на режим работы No Pull-Up, Pull-Down
    SET_BIT(RCC_GPIOA_BSRR, P4_LOW);                    // задаем нулевой сигнал на порте перед началом работы

    // Прямое обращение к памяти
    *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x2UL;     // включение тактирования шины GPIOB
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x1UL;     // настройка режима работы порта B0 на вывод сигнала
    *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x0UL;     // настройка типа выхода порта B0 на режим Output Push-Pull
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x1UL;     // настройка скорости работы порта B0 на среднюю
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x0UL;     // настраиваем подтяжки/стяжки пина B0 на режим No Pull-Up, Pull-Down
    *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x10000UL; // подаем нулевой сигнал на порт B0 перед началом работы

    // Использование библиотеки CMSIS
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;                                   // включение тактирования шины GPIOC
    GPIOC->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0;               // настройка режима работы порта C0 и С1 на вывод сигнала
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE13);                            // явная настройка порта С13 на прием сигнала
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT0 + GPIO_OTYPER_OT1);           // настройка типа выхода портов С0 и С1 на режим Output Push-Pull
    GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_0 + GPIO_OSPEEDER_OSPEEDR1_0; // настройка скорости работы портов С0 и С1 на среднюю
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0 | GPIO_PUPDR_PUPDR1);        // настраиваем подтяжки/стяжки пинов С0 и С1
                                                                           // на режим No Pull-Up, Pull-Down
    GPIOC->BSRR |= GPIO_BSRR_BR0 | GPIO_BSRR_BR1;                          // выключение диода перед началом
}

uint8_t time_skeeper(void)
{
    int pusher = 1;
    int smth = 0;
    for (int i = 0; i < 1; i++)
    {
        smth++;
    }
    return pusher;
}

uint8_t diode_mode(uint8_t pusher, uint8_t but_check)
{
    but_check++;
    if (but_check > 5)
    {
        but_check = 0;
    }
    return but_check;
}

void main_process(uint8_t but_check)
{
    switch (but_check)
    {
    case 0:
        RCC_GPIOA_BSRR |= P4_LOW;
        *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x10000UL;
        GPIOC->BSRR |= GPIO_BSRR_BR0 + GPIO_BSRR_BR1;
        first_led = 0;
        second_led = 0;
        third_led = 0;
        fourth_led = 0;
        break;
    case 1:
        RCC_GPIOA_BSRR |= P4_HIGH;
        *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x10000UL;
        GPIOC->BSRR |= GPIO_BSRR_BR0 + GPIO_BSRR_BR1;
        first_led = 1;
        second_led = 0;
        third_led = 0;
        fourth_led = 0;
        break;
    case 2:
        RCC_GPIOA_BSRR |= P4_LOW;
        *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x1UL;
        GPIOC->BSRR |= GPIO_BSRR_BR0 + GPIO_BSRR_BR1;
        first_led = 0;
        second_led = 1;
        third_led = 0;
        fourth_led = 0;
        break;
    case 3:
        RCC_GPIOA_BSRR |= P4_LOW;
        *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x10000UL;
        GPIOC->BSRR |= GPIO_BSRR_BR0 + GPIO_BSRR_BS1;
        first_led = 0;
        second_led = 0;
        third_led = 1;
        fourth_led = 0;
        break;
    case 4:
        RCC_GPIOA_BSRR |= P4_LOW;
        *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x10000UL;
        GPIOC->BSRR |= GPIO_BSRR_BS0 + GPIO_BSRR_BR1;
        first_led = 0;
        second_led = 0;
        third_led = 0;
        fourth_led = 1;
        break;
    case 5:
        *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x1UL;
        RCC_GPIOA_BSRR |= P4_HIGH;
        GPIOC->BSRR |= GPIO_BSRR_BS0 + GPIO_BSRR_BS1;
        first_led = 1;
        second_led = 1;
        third_led = 1;
        fourth_led = 1;
        break;
    }
}
