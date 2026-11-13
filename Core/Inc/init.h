#ifndef INIT_H
#define INIT_H

#ifndef STM32F429xx
#define STM32F429xx
#endif
#include "stm32f4xx.h"

int RCC_Init(void);
void GPIO_Init(void);
void DWT_Init(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

void INIT_UPRAVLENIE(GPIO_TypeDef *port, uint8_t n, uint8_t nf);

// fp здесь означает ДЕЛИТЕЛЬ частоты (например, 90 или 180), а не саму частоту
void INIT_TIM(volatile uint32_t *rcc_reg, uint32_t bit, TIM_TypeDef *TIMX, uint32_t fp, uint8_t chan);

#endif