// В этом файле происходит инициализация всех пинов на микроконтролере для 4 случаев
// Ниже нужно раскомментировать один из строк с #define, для выбора режима работы

#pragma once
#include "stdint.h"
// Выбор одной из реализаций:

// #define USE_REGISTERS_ONLY // Реализация задания напрямую через регистры 
// #define USE_MACROS_ONLY // Реализация задания через макросы и директивы
#define USE_CMSIS_ONLY // Реализация задания через библиотеку CMSIS
// #define ADD_TASK // Реализация дополнительного задания с использованием библиотеки CMSIS

#if defined(USE_REGISTERS_ONLY) // Реализация только через регистры

void GPIO_Init(void) {
    // Включение тактирования порта A (RCC AHB1 peripheral clock enable register)
    *(volatile unsigned int*)(0x40023800UL + 0x30UL) |= (1UL << 0);
    
    // Настройка светодиодов на порту A (PA2-PA7)
    for(int i = 2; i <= 7; i++) {
        // MODER - режим вывода (01 = Output mode)
        *(volatile unsigned int*)(0x40020000UL + 0x00UL) |= (1UL << (2 * i));
        // OTYPER - Push-Pull (0 = Push-Pull)
        *(volatile unsigned int*)(0x40020000UL + 0x04UL) &= ~(1UL << i);
        // OSPEEDR - Medium speed (01 = Medium speed)
        *(volatile unsigned int*)(0x40020000UL + 0x08UL) |= (1UL << (2 * i));
        // PUPDR - No pull-up/pull-down (00 = No pull-up, pull-down)
        *(volatile unsigned int*)(0x40020000UL + 0x0CUL) &= ~(3UL << (2 * i));
    }
    
    // Настройка кнопок на порту A (PA0 - кнопка 1, PA1 - кнопка 2)
    // PA0 - выбор количества светодиодов
    // MODER - Input mode (00 = Input mode)
    *(volatile unsigned int*)(0x40020000UL + 0x00UL) &= ~(3UL << (2 * 0));
    // PUPDR - Pull-down (10 = Pull-down)
    *(volatile unsigned int*)(0x40020000UL + 0x0CUL) &= ~(3UL << (2 * 0));
    *(volatile unsigned int*)(0x40020000UL + 0x0CUL) |= (2UL << (2 * 0));
    
    // PA1 - включение/выключение светодиодов
    // MODER - Input mode (00 = Input mode)
    *(volatile unsigned int*)(0x40020000UL + 0x00UL) &= ~(3UL << (2 * 1));
    // PUPDR - Pull-down (10 = Pull-down)
    *(volatile unsigned int*)(0x40020000UL + 0x0CUL) &= ~(3UL << (2 * 1));
    *(volatile unsigned int*)(0x40020000UL + 0x0CUL) |= (2UL << (2 * 1));
}

#elif defined(USE_MACROS_ONLY) // Реализация через макросы

// Макросы для адресов регистров
#define RCC_BASE        0x40023800UL
#define GPIOA_BASE      0x40020000UL

#define RCC_AHB1ENR     *(volatile unsigned int*)(RCC_BASE + 0x30UL)

#define GPIOA_MODER     *(volatile unsigned int*)(GPIOA_BASE + 0x00UL)
#define GPIOA_OTYPER    *(volatile unsigned int*)(GPIOA_BASE + 0x04UL)
#define GPIOA_OSPEEDR   *(volatile unsigned int*)(GPIOA_BASE + 0x08UL)
#define GPIOA_PUPDR     *(volatile unsigned int*)(GPIOA_BASE + 0x0CUL)
#define GPIOA_IDR       *(volatile unsigned int*)(GPIOA_BASE + 0x10UL)
#define GPIOA_ODR       *(volatile unsigned int*)(GPIOA_BASE + 0x14UL)

// Битовая маска
#define GPIOA_EN        (1UL << 0)

void GPIO_Init(void) {
    RCC_AHB1ENR |= GPIOA_EN;
    
    for(int i = 2; i <= 7; i++) {
        GPIOA_MODER |= (1UL << (2 * i));
        GPIOA_OTYPER &= ~(1UL << i);
        GPIOA_OSPEEDR |= (1UL << (2 * i));
        GPIOA_PUPDR &= ~(3UL << (2 * i));
    }
    
    GPIOA_MODER &= ~(3UL << (2 * 0));
    GPIOA_PUPDR &= ~(3UL << (2 * 0));
    GPIOA_PUPDR |= (2UL << (2 * 0));
    
    GPIOA_MODER &= ~(3UL << (2 * 1));
    GPIOA_PUPDR &= ~(3UL << (2 * 1));
    GPIOA_PUPDR |= (2UL << (2 * 1));
}

#elif defined(USE_CMSIS_ONLY) || defined(ADD_TASK) // Реализация через CMSIS (Дополнительное задание)

#include "stm32f411xe.h"

void GPIO_Init(void) {
    // Включение тактирования порта A
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    
    
    // Настройка светодиодов на порту A (PA2-PA7)
    for(int i = 2; i <= 7; i++) {
        // MODER - Output mode (01)
        GPIOA->MODER |= (1UL << (2 * i));
        // OTYPER - Push-Pull (0)
        GPIOA->OTYPER &= ~(1UL << i);
        // OSPEEDR - Medium speed (01)
        GPIOA->OSPEEDR |= (1UL << (2 * i));
        // PUPDR - No pull-up/pull-down (00)
        GPIOA->PUPDR &= ~(3UL << (2 * i));
    }
    
    // Настройка кнопок на порту A (PA0 - кнопка 1, PA1 - кнопка 2)
    // PA0 - выбор количества светодиодов
    GPIOA->MODER &= ~GPIO_MODER_MODER0_Msk;      // Input mode (00)
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0_Msk;       // Clear
    GPIOA->PUPDR |= (0x2 << GPIO_PUPDR_PUPD0_Pos); // Pull-down (10)
    
    // PA1 - включение/выключение светодиодов
    GPIOA->MODER &= ~GPIO_MODER_MODER1_Msk;      // Input mode (00)
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD1_Msk;       // Clear
    GPIOA->PUPDR |= (0x2 << GPIO_PUPDR_PUPD1_Pos); // Pull-down (10)
}

#endif