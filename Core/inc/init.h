#ifndef LAB2_INIT_H
#define LAB2_INIT_H

#include "stm32f4xx.h"

#define LED_PINS ((0xFU << 12U) | (0x3U << 8U))

void Board_Init(void);

#endif