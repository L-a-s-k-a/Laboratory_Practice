#include "init.h"

// 1 Прямое обращение к  памяти
void _init_mem() {
    // Включаем подтяжку Pull-up для кнопок PA5 и PA6 напрямую через адреса
    *(uint32_t*)(0x40020000UL + 0x0CUL) &= ~((0x3UL << 10) | (0x3UL << 12)); 
    *(uint32_t*)(0x40020000UL + 0x0CUL) |= ((1UL << 10) | (1UL << 12));  
}

// 2 Конфигурация через собственные макросы
void _init_myself_def() {
    // Переводим кнопки PA5 и PA6 в режим Входа (00) через макрос
    MY_GPIOA_MODER &= ~MY_GPIOA_INPUT_5_6;
}

// 3 Конфигурация CMSIS
void _init_CMSIS() {
    // Активируем тактирование Портов А и B
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);

    // Настраиваем светодиоды PB3, PB4, PB5 на выход (код 01)
    GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER4 | GPIO_MODER_MODER5);
    GPIOB->MODER |= (GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0);
}
