// init.h
#include "init.h"

// Функция для задержки
void delay(volatile uint32_t count) {
    for (; count > 0; count--);
}

// Настройка PA5 для светодиода LED1
void GPIO_Init_with_memory_for_led5(void) 
{
    // Включаем тактирование порта A
    *ADDR_RCC_AHB1ENR |= (1UL << 0);
    
    // Настраиваем PA5 как выход
    *ADDR_GPIOA_MODER &= ~(0x3UL << (5 * 2));
    *ADDR_GPIOA_MODER |= (0x1UL << (5 * 2));
    
    // Тип выхода - push-pull
    *ADDR_GPIOA_OTYPER &= ~(1UL << 5);
    
    // Скорость высокая
    *ADDR_GPIOA_OSPEEDR &= ~(0x3UL << (5 * 2));
    *ADDR_GPIOA_OSPEEDR |= (0x2UL << (5 * 2));
    
    // Без подтяжки
    *ADDR_GPIOA_PUPDR &= ~(0x3UL << (5 * 2));
}

// Настройка через CMSIS (готовая библиотека)
void GPIO_Init_with_CMSIS(void) 
{
    // Включаем порты A и B
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    
    // Кнопка PB1
    GPIOB->MODER &= ~(0x3 << (1 * 2));  
    GPIOB->PUPDR &= ~(0x3 << (1 * 2));  
    GPIOB->PUPDR |= (0x1 << (1 * 2));
    
    // Кнопка PB2
    GPIOB->MODER &= ~(0x3 << (2 * 2));  
    GPIOB->PUPDR &= ~(0x3 << (2 * 2));  
    GPIOB->PUPDR |= (0x1 << (2 * 2));
    
    // Кнопка PB3
    GPIOB->MODER &= ~(0x3 << (3 * 2));  
    GPIOB->PUPDR &= ~(0x3 << (3 * 2));  
    GPIOB->PUPDR |= (0x1 << (3 * 2));
    
    // Кнопка PB8
    GPIOB->MODER &= ~(0x3 << (8 * 2));  
    GPIOB->PUPDR &= ~(0x3 << (8 * 2));  
    GPIOB->PUPDR |= (0x1 << (8 * 2));
    
    // Светодиод PA10
    GPIOA->MODER &= ~(0x3 << (10 * 2));
    GPIOA->MODER |=  (0x1 << (10 * 2));
    GPIOA->OTYPER &= ~(1 << 10);
    GPIOA->OSPEEDR &= ~(0x3 << (10 * 2));
    GPIOA->OSPEEDR |= (0x2 << (10 * 2));
    GPIOA->PUPDR &= ~(0x3 << (10 * 2));
    
    // Светодиод PA11
    GPIOA->MODER &= ~(0x3 << (11 * 2));
    GPIOA->MODER |=  (0x1 << (11 * 2));
    GPIOA->OTYPER &= ~(1 << 11);
    GPIOA->OSPEEDR &= ~(0x3 << (11 * 2));
    GPIOA->OSPEEDR |= (0x2 << (11 * 2));
    GPIOA->PUPDR &= ~(0x3 << (11 * 2));
}

// Настройка через свои макросы
void GPIO_Init_with_my_macro(void) 
{
    // Включаем тактирование
    RCC_GPIO |= RCC_GPIOA_ON | RCC_GPIOB_ON;
    
    // Кнопки PB1, PB2, PB3, PB8
    GPIOB_MODER = (GPIOB_MODER & ~(0x3UL << (1 * 2)));
    GPIOB_MODER = (GPIOB_MODER & ~(0x3UL << (2 * 2)));
    GPIOB_MODER = (GPIOB_MODER & ~(0x3UL << (3 * 2)));
    GPIOB_MODER = (GPIOB_MODER & ~(0x3UL << (8 * 2)));
    GPIOB_MODER |= GPIOB_MODE_PIN1_IN | GPIOB_MODE_PIN2_IN | 
                   GPIOB_MODE_PIN3_IN | GPIOB_MODE_PIN8_IN;
    
    GPIOB_PUPDR = (GPIOB_PUPDR & ~(0x3UL << (1 * 2)));
    GPIOB_PUPDR = (GPIOB_PUPDR & ~(0x3UL << (2 * 2)));
    GPIOB_PUPDR = (GPIOB_PUPDR & ~(0x3UL << (3 * 2)));
    GPIOB_PUPDR = (GPIOB_PUPDR & ~(0x3UL << (8 * 2)));
    GPIOB_PUPDR |= GPIOB_PUPD_PIN1_PULL_UP | GPIOB_PUPD_PIN2_PULL_UP | 
                   GPIOB_PUPD_PIN3_PULL_UP | GPIOB_PUPD_PIN8_PULL_UP;
    
    // Светодиод PA10
    GPIOA_MODER |= GPIOA_MODE_PIN10_OUT;
    GPIOA_OTYPER |= GPIOA_OTYPE_PIN10_PP;
    GPIOA_OSPEEDR |= GPIOA_OSPEED_PIN10_HS;
    GPIOA_PUPDR |= GPIOA_PUPD_PIN10_NOPUPD;
    
    // Светодиод PA11
    GPIOA_MODER |= GPIOA_MODE_PIN11_OUT;
    GPIOA_OTYPER |= GPIOA_OTYPE_PIN11_PP;
    GPIOA_OSPEEDR |= GPIOA_OSPEED_PIN11_HS;
    GPIOA_PUPDR |= GPIOA_PUPD_PIN11_NOPUPD;
}