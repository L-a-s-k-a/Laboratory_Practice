#include <init.h>

// Способ 1: прямые адреса. Порт B: PB9 - вход с подтяжкой (кнопка)
void __init_mem(void)
{
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x40000UL; // GPIOB PUPDR PB9 pull-up
}

// Способ 2: свои макросы. Тактирование A, B, C и порт C: PC7 - вход с подтяжкой
void __init_myself_def(void)
{
    BIT_SET(RCC_AHB1ENR, RCC_GPIOA_EN | RCC_GPIOB_EN | RCC_GPIOC_EN); // включили тактирование для портов
    BIT_SET(GPIOC_PUPDR, GPIO_PULLUP_7);                              // включили подтяжку к питанию на кнопку
}

// Способ 3: CMSIS. Порт A: PA5, PA6, PA7 - выходы (светодиоды)
void __init_CMSIS(void)
{
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
}

// ДЛЯ ДОП ЗАДАНИЯ
void __dop_zadanie_init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);                       // включаем тактирование
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER7_0);                                             // Светодиод PC7 - выход
    SET_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR6_0); // Включаем подтяжку к питанию на кнопки PB9, PB8, PB6 
}