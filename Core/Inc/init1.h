#ifndef INIT1_H
#define INIT1_H

#include "stm32f4xx.h"

#define LED_COUNT 6U

typedef struct {
    GPIO_TypeDef *port;
    uint8_t pin;
} Led_t;

extern const Led_t Leds[LED_COUNT];

void RCC_Ini(void);
void GPIO_Ini(void);
void EXTI_ITR_Ini(void);
void SysTick_Init(void);

void Led_On(uint8_t index);
void Led_Off(uint8_t index);
void Leds_AllOff(void);

#endif
