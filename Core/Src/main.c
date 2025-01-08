#include "init.h"
#include "it_handlers.h"
#include "../../CMSIS/Devices/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/Inc/stm32f429xx.h"

// Custom millisecond counter
volatile uint32_t msTicks = 0;

void SysTick_Handler(void) {
    msTicks++;
}

uint32_t getMillis() {
    return msTicks;
}

// LED and timing constants
#define NUM_LEDS 6
#define LONG_PRESS_TIME 2000 // 2 seconds
#define SHORT_PRESS_TIME 50  // 50ms debounce

// Frequency ranges (in milliseconds)
const uint16_t freqRanges[3][3] = {
    {3333, 1000, 714},   // 0.3Hz, 1Hz, 1.4Hz
    {2500, 833, 588},    // 0.4Hz, 1.2Hz, 1.7Hz
    {2000, 625, 526}     // 0.5Hz, 1.6Hz, 1.9Hz
};

// Global state variables
uint16_t GlobalTickCount;      // Global timer
uint8_t currentLED = 0;        // Currently selected LED (0-5)
uint8_t ledStates = 0;         // Bitmask for LED states (1=on)
uint8_t currentFreqRange = 0;  // Current frequency range index (0-2)
uint8_t currentFreqIndex = 0;  // Current frequency index within range (0-2)
uint32_t button1PressTime = 0; // Button 1 press timing
uint32_t button2PressTime = 0; // Button 2 press timing
uint8_t button1Pressed = 0;    // Button 1 pressed state
uint8_t button2Pressed = 0;    // Button 2 pressed state

// Helper function to set LED states
void updateLEDs()
{
    for (uint8_t i = 0; i < NUM_LEDS; i++)
    {
        if (ledStates & (1 << i))
        {
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0 << (i * 7)); // Turn on LED
        }
        else
        {
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0 << (i * 7)); // Turn off LED
        }
    }
}

// Helper function to handle button 1 presses
void handleButton1()
{
    static uint32_t lastPressTime = 0;
    uint32_t currentTime = getMillis();

    if (button1Pressed)
    {
        if (currentTime - button1PressTime >= LONG_PRESS_TIME)
        {
            // Long press - cycle LEDs
            if (ledStates == 0x3F)
            {                  // All LEDs on
                ledStates = 0; // Turn all off
            }
            else
            {
                ledStates |= (1 << currentLED); // Turn on next LED
                currentLED = (currentLED + 1) % NUM_LEDS;
            }
            updateLEDs();
            button1Pressed = 0;
        }
    }
    else if (currentTime - lastPressTime > SHORT_PRESS_TIME)
    {
        // Short press - change frequency
        currentFreqIndex = (currentFreqIndex + 1) % 3;
        lastPressTime = currentTime;
    }
}

// Helper function to handle button 2 presses
void handleButton2()
{
    static uint32_t lastPressTime = 0;
    uint32_t currentTime = getMillis();

    if (button2Pressed)
    {
        if (currentTime - button2PressTime >= LONG_PRESS_TIME)
        {
            // Long press - change LED selection
            currentLED = (currentLED + 1) % NUM_LEDS;
            button2Pressed = 0;
        }
    }
    else if (currentTime - lastPressTime > SHORT_PRESS_TIME)
    {
        // Short press - change frequency range
        currentFreqRange = (currentFreqRange + 1) % 3;
        lastPressTime = currentTime;
    }
}

int main(void)
{
    GPIO_Ini();     // Initialize GPIO
    RCC_Ini();      // Initialize RCC clock
    EXTI_ITR_Ini(); // Initialize interrupt controller
    SysTick_Init(); // Initialize system timer

    while (1)
    {
        // Handle button presses
        handleButton1();
        handleButton2();

        // Update LED blinking based on current frequency
        uint16_t currentPeriod = freqRanges[currentFreqRange][currentFreqIndex];
        if (GlobalTickCount >= currentPeriod)
        {
            GlobalTickCount = 0;
            ledStates ^= (1 << currentLED); // Toggle current LED
            updateLEDs();
        }
    }
}
