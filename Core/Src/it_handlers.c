#include "it_handlers.h"
#include "init.h"

// Declare getMillis function
uint32_t getMillis(void);

// External variables from main.c
extern uint8_t button1Pressed;
extern uint8_t button2Pressed;
extern uint32_t button1PressTime;
extern uint32_t button2PressTime;


void EXTI15_10_IRQHandler(void) {
    uint32_t currentTime = getMillis();
    
    // Check which button triggered the interrupt
    if (READ_BIT(EXTI->PR, EXTI_PR_PR13)) { // Button 1
        SET_BIT(EXTI->PR, EXTI_PR_PR13); // Clear interrupt flag
        if (!button1Pressed) {
            button1Pressed = 1;
            button1PressTime = currentTime;
        }
    }
    
    if (READ_BIT(EXTI->PR, EXTI_PR_PR14)) { // Button 2
        SET_BIT(EXTI->PR, EXTI_PR_PR14); // Clear interrupt flag
        if (!button2Pressed) {
            button2Pressed = 1;
            button2PressTime = currentTime;
        }
    }
}

void EXTI9_5_IRQHandler(void) {
    // Handle button release
    if (READ_BIT(EXTI->PR, EXTI_PR_PR13)) { // Button 1 release
        SET_BIT(EXTI->PR, EXTI_PR_PR13);
        button1Pressed = 0;
    }
    
    if (READ_BIT(EXTI->PR, EXTI_PR_PR14)) { // Button 2 release
        SET_BIT(EXTI->PR, EXTI_PR_PR14);
        button2Pressed = 0;
    }
}

void User_Delay(uint32_t delay) {
    uint32_t start = getMillis();
    while (getMillis() - start < delay) {}
}
