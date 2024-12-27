#include "init.h"

void GPIO_Init(void) {
    // Enable GPIOC clock
    RCC_GPIO_EN |= RCC_GPIOC_EN;

    // Configure LEDs (PC8, PC9, PC10, PC11) as output
    GPIOC_MODER |= (0x01UL << 16) | (0x01UL << 18) | (0x01UL << 20) | (0x01UL << 22);

    // Configure Buttons (PC8, PC9) as input
    GPIOC_MODER &= ~(0x03UL << 16); // PC8 input
    GPIOC_MODER &= ~(0x03UL << 18); // PC9 input

    // No pull-up/pull-down for buttons
    GPIOC_PUPDR &= ~(0x03UL << 16); 
    GPIOC_PUPDR &= ~(0x03UL << 18); 
}

void delay(uint32_t s) {
    while (s--) {
        __NOP();
    }
}
