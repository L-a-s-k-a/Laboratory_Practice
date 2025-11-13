#include "../Inc/init.h"

#include "init.h"

void GPIO_Ini(void)
{
    // ==========================================================================
    // STEP 1: ENABLE CLOCKS FOR GPIO PORTS
    // ==========================================================================
    // Microcontroller needs clock signals to make GPIO ports work
    // RCC->AHB1ENR controls clocks for GPIO ports B and C
    // RCC_AHB1ENR_GPIOBEN enables clock for GPIO Port B (where LEDs are)
    // RCC_AHB1ENR_GPIOCEN enables clock for GPIO Port C (where buttons are)
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    
    // ==========================================================================
    // STEP 2: CONFIGURE ON-BOARD LEDs (PB0, PB7, PB14)
    // ==========================================================================
    
    // LED1 - PB0 (Green LED on Nucleo board)
    // GPIO_MODER register sets pin mode: 00=Input, 01=Output, 10=Alternate, 11=Analog
    CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODER0);   // Clear both mode bits first
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER0_0);   // Set to 01 = Output mode
    
    // LED2 - PB7 (Blue LED on Nucleo board)
    CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODER7);   // Clear both mode bits
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER7_0);   // Set to 01 = Output mode
    
    // LED3 - PB14 (Red LED on Nucleo board)
    CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODER14);  // Clear both mode bits
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER14_0);  // Set to 01 = Output mode
    
    // ==========================================================================
    // STEP 3: CONFIGURE EXTERNAL LEDs (PB1, PB2, PB3)
    // ==========================================================================
    // These are the additional LEDs you need to connect externally
    for (int pin = 1; pin <= 3; pin++) {
        // Clear both mode bits (make them 00)
        CLEAR_BIT(GPIOB->MODER, (0x3UL << (pin * 2)));
        // Set to output mode (01)
        SET_BIT(GPIOB->MODER, (0x1UL << (pin * 2)));
    }
    
    // ==========================================================================
    // STEP 4: CONFIGURE ALL LED PROPERTIES
    // ==========================================================================
    for (int pin = 0; pin <= 15; pin++) {
        // Only configure pins we're using: PB0, PB1, PB2, PB3, PB7, PB14
        if (pin == 0 || pin == 1 || pin == 2 || pin == 3 || pin == 7 || pin == 14) {
            // GPIO_OTYPER: Output Type Register
            // 0 = Push-Pull (can drive HIGH and LOW strongly)
            // 1 = Open-Drain (can only pull LOW, needs external resistor for HIGH)
            CLEAR_BIT(GPIOB->OTYPER, (1UL << pin));  // Set to Push-Pull
            
            // GPIO_OSPEEDR: Output Speed Register  
            // 00 = Low speed (2 MHz), 01 = Medium speed (25-50 MHz)
            // 10 = High speed (50-100 MHz), 11 = Very high speed (180 MHz)
            CLEAR_BIT(GPIOB->OSPEEDR, (0x3UL << (pin * 2)));  // Clear speed bits
            SET_BIT(GPIOB->OSPEEDR, (0x1UL << (pin * 2)));    // Set to Medium speed
            
            // GPIO_PUPDR: Pull-up/Pull-down Register
            // 00 = No pull-up or pull-down, 01 = Pull-up, 10 = Pull-down
            CLEAR_BIT(GPIOB->PUPDR, (0x3UL << (pin * 2)));    // No pull-up/pull-down
        }
    }
    
    // ==========================================================================
    // STEP 5: CONFIGURE BUTTON 1 (PB4 - EXTERNAL BUTTON)
    // ==========================================================================
    // CHANGED: From PC13 to PB4
    // GPIO_MODER: Set to input mode (00)
    CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODER4);
    
    // GPIO_PUPDR: Enable pull-down resistor
    // When button is not pressed, pin reads 0 (LOW)
    // When button is pressed, pin reads 1 (HIGH) because button connects to 3.3V
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR4_0);  // Clear first bit
    SET_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR4_1);    // Set second bit = Pull-down
    
    // ==========================================================================
    // STEP 6: CONFIGURE BUTTON 2 (PB5 - EXTERNAL BUTTON)
    // ==========================================================================
    // CHANGED: From PC14 to PB5
    // Same configuration as Button 1
    CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODER5);    // Input mode
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR5_0);  // Clear first bit  
    SET_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR5_1);    // Set second bit = Pull-down
    
    // ==========================================================================
    // STEP 7: INITIAL STATE - ALL LEDs OFF
    // ==========================================================================
    // GPIO_BSRR: Bit Set/Reset Register
    // Writing 1 to lower 16 bits sets the pin HIGH (LED ON)
    // Writing 1 to upper 16 bits sets the pin LOW (LED OFF)
    // We use (1 << pin) << 16 to set the reset bit (turn LED OFF)
    GPIOB->BSRR = (1UL << 0) << 16;   // PB0 OFF - Green LED
    GPIOB->BSRR = (1UL << 1) << 16;   // PB1 OFF - External LED 1
    GPIOB->BSRR = (1UL << 2) << 16;   // PB2 OFF - External LED 2
    GPIOB->BSRR = (1UL << 3) << 16;   // PB3 OFF - External LED 3
    GPIOB->BSRR = (1UL << 7) << 16;   // PB7 OFF - Blue LED
    GPIOB->BSRR = (1UL << 14) << 16;  // PB14 OFF - Red LED
    }