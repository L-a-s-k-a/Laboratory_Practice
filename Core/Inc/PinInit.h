#pragma once

#include <stdint.h>
#include "stm32f4xx.h"

#define RCC_BASE        0x40023800UL
#define RCC_AHB1ENR     (*(volatile unsigned long *)(RCC_BASE + 0x30))

// --- Порт C ---
#define GPIOC_BASE       0x40020800UL
#define PC1              1
#define PC11             11
#define PC12             12

// --- Порт D ---
#define GPIOD_MODER      (GPIOD->MODER)
#define GPIOD_ODR        (GPIOD->ODR)
#define PD1              1

// --- Порт E ---
#define GPIOE_BASE       0x40021000UL
#define GPIOE_MODER      (*(volatile uint32_t *)(GPIOE_BASE + 0x00))
#define GPIOE_ODR        (*(volatile uint32_t *)(GPIOE_BASE + 0x14))
#define PE5              5

// ------------------ ФУНКЦИИ ------------------
void pcSet(int pin);
void pdSet(int pin);
void peSet(int pin);

void pcReset(int pin);
void peReset(int pin);
void pdReset(int pin);

uint8_t readPC(int pin);

void pcOutput(int pin);
void pdOutput(int pin);
void peOutput(int pin);

void pc_SetInput(int pin);

void peToggle(int pin);


