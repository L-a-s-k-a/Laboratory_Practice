#include "it_handlers.h"

extern uint8_t current_mode;
extern uint8_t current_led;
extern uint8_t current_frequency;
extern volatile uint32_t systick_counter;

void SysTick_Handler(void) {
    systick_counter++;
}

void EXTI9_5_IRQHandler(void) {
    if (READ_BIT(EXTI->PR, EXTI_PR_PR8)) {
        current_mode = (current_mode % 2) + 1;
        SET_BIT(EXTI->PR, EXTI_PR_PR8);
    }
    if (READ_BIT(EXTI->PR, EXTI_PR_PR9)) {
        current_frequency = (current_frequency + 1) % 3;
        SET_BIT(EXTI->PR, EXTI_PR_PR9);
    }
    if (READ_BIT(EXTI->PR, EXTI_PR_PR10)) {
        current_led = (current_led + 1) % 6;
        SET_BIT(EXTI->PR, EXTI_PR_PR10);
    }
}
