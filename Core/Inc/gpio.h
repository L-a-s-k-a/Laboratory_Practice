#ifndef GPIO_H
#define GPIO_H
#include "main.h"

#define DELAY 35
#define DELAY_LONG 145

extern void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t Pin, uint8_t State);
extern uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t Pin);
extern void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t Pin);

#endif
