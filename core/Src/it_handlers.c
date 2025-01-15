#include "it_handlers.h"

// External variables
extern uint8_t current_led; // Index of the currently lit LED
extern uint32_t systick_counter; // SysTick counter

uint16_t DelayTickCount; // Counter for delay function
uint32_t ExternInterruptTickCount; // Counter for external interrupt debounce

// SysTick interrupt handler
void SysTick_Handler(void) {
    systick_counter++; // Increment 1ms counter
    DelayTickCount++; // Increment delay counter
    ExternInterruptTickCount++; // Increment external interrupt debounce counter
}

// Button 1 (PC13) interrupt handler
void EXTI15_10_IRQHandler(void) {
    // Clear the interrupt pending bit
    SET_BIT(EXTI->PR, EXTI_PR_PR13);

    // Debounce check
    if (ExternInterruptTickCount >= DELAY_BUTTON_FILTER) {
        current_led = (current_led + 1) % 6; // Switch to the next LED
        update_led_state(); // Update the LED state
        ExternInterruptTickCount = 0; // Reset debounce counter
    }
}

// Update the state of the LEDs
void update_led_state(void) {
    // Turn off all LEDs
    GPIOB->BSRR = LED1_PIN << 16 | LED2_PIN << 16 | LED3_PIN << 16 |
                  LED4_PIN << 16 | LED5_PIN << 16 | LED6_PIN << 16;

    // Light up the current LED
    switch (current_led) {
        case 0: SET_BIT(GPIOB->BSRR, LED1_PIN); break;
        case 1: SET_BIT(GPIOB->BSRR, LED2_PIN); break;
        case 2: SET_BIT(GPIOB->BSRR, LED3_PIN); break;
        case 3: SET_BIT(GPIOB->BSRR, LED4_PIN); break;
        case 4: SET_BIT(GPIOB->BSRR, LED5_PIN); break;
        case 5: SET_BIT(GPIOB->BSRR, LED6_PIN); break;
    }
}

// Uncomment and implement the delay function if needed
// void User_Delay(uint32_t delay) {
//     while (DelayTickCount < delay) {} // Busy-wait for the specified delay
//     if (DelayTickCount >= delay) DelayTickCount = 0; // Reset delay counter
// }
