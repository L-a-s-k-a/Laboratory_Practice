#include <stdint.h>
#include "init.h"

void delay(void);
//void UpdateLEDs(uint8_t count);   // 更新LED状态
void UpdateModeLEDs(uint8_t mode, uint8_t button);  // 根据模式和按钮更新LED状态
void UpdateModeLEDoff(uint8_t mode, uint8_t button);  // 根据模式和按钮更新LED状态

volatile uint8_t mode = 0;  // 当前模式，0、1、2

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOBEN;

    // THE LED SETTINGS
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE8_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE9_0);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE10_0);

    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_8);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_9);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_10);

    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR9_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR10_0);

    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR8_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR9_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR10_0);

    // THE BUTTON SETTINGS PC13 (Mode switch button)
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE13);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD13_1);

    // THE BUTTON SETTINGS PC6, PC8, PC9 (Control buttons)
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE6);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD6_1);

    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE8);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD8_1);

    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE9);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD9_1);

    while (1)
    {
        // Handle PC13 press to switch mode
        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)) != 0) // PC13 is pressed
        {
            delay(); // Debouncing
            if ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)) != 0) // PC13 is still pressed
            {
                mode = (mode + 1) % 3; // Switch between 0, 1, 2
                while ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)) != 0) ; // Wait for button release
            }
        }

        // Handle PC6, PC8, PC9 to control LEDs based on current mode
        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_6)) != 0) // PC6 is pressed
        {            
                UpdateModeLEDs(mode, 6); // Update LEDs based on the current mode and PC6 press               
        }
        else{ 
                UpdateModeLEDsoff(mode, 6);
        }

        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_8)) != 0) // PC6 is pressed
        {            
                UpdateModeLEDs(mode, 8); // Update LEDs based on the current mode and PC6 press               
        }
        else{ 
                UpdateModeLEDsoff(mode, 8);
        }

        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_9)) != 0) // PC6 is pressed
        {            
                UpdateModeLEDs(mode, 9); // Update LEDs based on the current mode and PC6 press               
        }
        else{ 
                UpdateModeLEDsoff(mode, 9);
        }
    }

    return 0;
}

// 根据当前模式和按钮更新LED状态
void UpdateModeLEDs(uint8_t mode, uint8_t button)
{
    // 先关闭所有LED
    GPIOB->BSRR = (GPIO_BSRR_BR0 | GPIO_BSRR_BR7 | GPIO_BSRR_BR14);
    GPIOB->BSRR = (GPIO_BSRR_BR8 | GPIO_BSRR_BR9 | GPIO_BSRR_BR10);

    if (mode == 0) {
        // Mode 0: Define button LED behavior
        if (button == 6) {
            // PC6 controls PB0, PB8
            GPIOB->BSRR = GPIO_BSRR_BS0;  // PB0
            GPIOB->BSRR = GPIO_BSRR_BS8;  // PB8
        }
        else if (button == 8) {
            // PC8 controls PB7, PB9
            GPIOB->BSRR = GPIO_BSRR_BS7;  // PB7
            GPIOB->BSRR = GPIO_BSRR_BS9;  // PB9
        }
        else if (button == 9) {
            // PC9 controls PB14, PB10
            GPIOB->BSRR = GPIO_BSRR_BS14; // PB14
            GPIOB->BSRR = GPIO_BSRR_BS10; // PB10
        }
    }
    else if (mode == 1) {
        // Mode 1: Define button LED behavior
        if (button == 6) {
            // PC6 controls PB7, PB9
            GPIOB->BSRR = GPIO_BSRR_BS7;  // PB7
            GPIOB->BSRR = GPIO_BSRR_BS9;  // PB9
        }
        else if (button == 8) {
            // PC8 controls PB14, PB10
            GPIOB->BSRR = GPIO_BSRR_BS14; // PB14
            GPIOB->BSRR = GPIO_BSRR_BS10; // PB10
        }
        else if (button == 9) {
            // PC9 controls PB0, PB8
            GPIOB->BSRR = GPIO_BSRR_BS0;  // PB0
            GPIOB->BSRR = GPIO_BSRR_BS8;  // PB8
        }
    }
    else if (mode == 2) {
        // Mode 2: Define button LED behavior
        if (button == 6) {
            // PC6 controls PB14, PB10
            GPIOB->BSRR = GPIO_BSRR_BS14; // PB14
            GPIOB->BSRR = GPIO_BSRR_BS10; // PB10
        }
        else if (button == 8) {
            // PC8 controls PB0, PB8
            GPIOB->BSRR = GPIO_BSRR_BS0;  // PB0
            GPIOB->BSRR = GPIO_BSRR_BS8;  // PB8
        }
        else if (button == 9) {
            // PC9 controls PB7, PB9
            GPIOB->BSRR = GPIO_BSRR_BS7;  // PB7
            GPIOB->BSRR = GPIO_BSRR_BS9;  // PB9
        }
    }
}

void UpdateModeLEDsoff(uint8_t mode, uint8_t button)
{
    

    if (mode == 0) {
        // Mode 0: Define button LED behavior
        if (button == 6) {
            // PC6 controls PB0, PB8
            GPIOB->BSRR = GPIO_BSRR_BR0;  // PB0
            GPIOB->BSRR = GPIO_BSRR_BR8;  // PB8
        }
        else if (button == 8) {
            // PC8 controls PB7, PB9
            GPIOB->BSRR = GPIO_BSRR_BR7;  // PB7
            GPIOB->BSRR = GPIO_BSRR_BR9;  // PB9
        }
        else if (button == 9) {
            // PC9 controls PB14, PB10
            GPIOB->BSRR = GPIO_BSRR_BR14; // PB14
            GPIOB->BSRR = GPIO_BSRR_BR10; // PB10
        }
    }
    else if (mode == 1) {
        // Mode 1: Define button LED behavior
        if (button == 6) {
            // PC6 controls PB7, PB9
            GPIOB->BSRR = GPIO_BSRR_BR7;  // PB7
            GPIOB->BSRR = GPIO_BSRR_BR9;  // PB9
        }
        else if (button == 8) {
            // PC8 controls PB14, PB10
            GPIOB->BSRR = GPIO_BSRR_BR14; // PB14
            GPIOB->BSRR = GPIO_BSRR_BR10; // PB10
        }
        else if (button == 9) {
            // PC9 controls PB0, PB8
            GPIOB->BSRR = GPIO_BSRR_BR0;  // PB0
            GPIOB->BSRR = GPIO_BSRR_BR8;  // PB8
        }
    }
    else if (mode == 2) {
        // Mode 2: Define button LED behavior
        if (button == 6) {
            // PC6 controls PB14, PB10
            GPIOB->BSRR = GPIO_BSRR_BR14; // PB14
            GPIOB->BSRR = GPIO_BSRR_BR10; // PB10
        }
        else if (button == 8) {
            // PC8 controls PB0, PB8
            GPIOB->BSRR = GPIO_BSRR_BR0;  // PB0
            GPIOB->BSRR = GPIO_BSRR_BR8;  // PB8
        }
        else if (button == 9) {
            // PC9 controls PB7, PB9
            GPIOB->BSRR = GPIO_BSRR_BR7;  // PB7
            GPIOB->BSRR = GPIO_BSRR_BR9;  // PB9
        }
    }
}

// 延迟函数，用于消抖
void delay(void)
{
    for (volatile int i = 0; i < 100000; i++)
    {
        __asm__("nop");
    }
}
