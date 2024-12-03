#include "gpio.h"
void GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t Pin, uint8_t State) {
    if (State) {
        GPIOx->BSRR = (1 << Pin);
    } else {
        GPIOx->BSRR = (1 << (Pin + 16));
    }
}

uint8_t GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t Pin) {
    return (GPIOx->IDR & (1 << Pin)) ? 1 : 0;
}

void GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t Pin) {
    uint8_t currentState = GPIO_ReadPin(GPIOx, Pin);
    GPIO_WritePin(GPIOx, Pin, !currentState);
}
