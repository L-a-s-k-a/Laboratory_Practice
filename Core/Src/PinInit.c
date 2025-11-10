#include "PinInit.h"

void pcOutput(int pin) {
    (*(volatile uint32_t *)(0x40020800UL + 0x00)) &= ~(3U << (pin * 2));
    (*(volatile uint32_t *)(0x40020800UL + 0x00)) |=  (1U << (pin * 2));
}
void pcSet(int pin) { (*(volatile uint32_t *)(0x40020800UL + 0x14)) |= (1U << pin); }

void pc_SetInput(int pin) {
    // Очистка соответствующих битов MODER11 и MODER12
    (*(volatile uint32_t *)(0x40020800UL + 0x00)) &= ~(3U << (pin * 2));
    // 00
}

// Считывание состояния кнопок
// Возвращает 0, если кнопка отпущена, 1 если нажата
uint8_t readPC(int pin) {
    return ((*(volatile uint32_t *)(GPIOC_BASE + 0x10)) & (1U << pin)) ? 1 : 0;
}
void pcReset(int pin)   { (*(volatile uint32_t *)(GPIOC_BASE + 0x14)) &= ~(1U << pin); }

// Настройка и включение пина PD1
void pdOutput(int pin) {
    GPIOD_MODER &= ~(3U << (pin * 2));
    GPIOD_MODER |=  (1U << (pin * 2));
}
void pdSet(int pin) { GPIOD_ODR |= (1U << pin); }

// Настройка и включение пина PE
void peOutput(int pin) {
    GPIOE_MODER &= ~(3U << (pin * 2));
    GPIOE_MODER |=  (1U << (pin * 2));
}
// Выключить PE (0)
void peReset(int pin) {
    GPIOE_ODR &= ~(1U << pin);
}
void pdReset(int pin)   { GPIOD_ODR &= ~(1U << pin); }
// Переключить состояние PE
void peToggle(int pin) {
    GPIOE_ODR ^= (1U << pin);
}
void peSet(int pin) { GPIOE_ODR |= (1U << pin); }
