
#include "../Inc/init.h"

#include "init.h"
#include <stdint.h>

// ==========================================================================
// GLOBAL VARIABLES FOR SYSTEM STATE
// ==========================================================================

// LED management
uint8_t current_led_state = 0;        // Bitmask of currently lit LEDs
uint8_t led_sequence_step = 0;        // Current step in LED sequence (0-6)
const uint8_t LED_PINS[6] = {0, 7, 14, 1, 2, 3}; // PB0, PB7, PB14, PB1, PB2, PB3

// Blinking management for each LED
uint8_t blink_mode[6] = {0, 0, 0, 0, 0, 0}; // Blink mode for each LED: 0=Constant ON, 1=Low, 2=Medium, 3=High
uint32_t last_blink_time[6] = {0, 0, 0, 0, 0, 0}; // Last blink time for each LED
uint8_t blink_state[6] = {0, 0, 0, 0, 0, 0}; // Current blink state for each LED (0=OFF, 1=ON)
uint32_t blink_intervals[4] = {0, 1000, 500, 200}; // Blink intervals in ms (Constant ON, Low, Medium, High)

// Button debouncing
uint32_t last_button1_time = 0;       // Last time button 1 was pressed
uint32_t last_button2_time = 0;       // Last time button 2 was pressed
const uint32_t DEBOUNCE_DELAY = 200;  // Debounce delay in ms

// System timer
volatile uint32_t system_tick = 0;    // System tick counter (incremented in SysTick_Handler)

// ==========================================================================
// FUNCTION DECLARATIONS
// ==========================================================================
void update_leds(void);
void handle_blinking(void);
uint8_t is_button_pressed(GPIO_TypeDef* port, uint16_t pin, uint32_t* last_press_time);
void SysTick_Handler(void);
uint32_t get_current_time(void);

// ==========================================================================
// SYSTEM INITIALIZATION
// ==========================================================================

/**
 * @brief Initialize System Tick timer for timing functions
 * @param none
 * @return none
 */
void SystemTick_Init(void) {
    // Configure SysTick to generate interrupt every 1ms
    // SystemCoreClock is typically 16MHz for HSI
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;  // Set reload value for 1ms
    SysTick->VAL = 0;                              // Clear current value
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |   // Use processor clock
                    SysTick_CTRL_TICKINT_Msk |     // Enable SysTick interrupt
                    SysTick_CTRL_ENABLE_Msk;       // Enable SysTick
}

/**
 * @brief SysTick Interrupt Handler - called every 1ms
 * @param none
 * @return none
 */
void SysTick_Handler(void) {
    system_tick++;  // Increment system tick counter
}

/**
 * @brief Get current time in milliseconds
 * @param none
 * @return Current time in milliseconds
 */
uint32_t get_current_time(void) {
    return system_tick;
}

// ==========================================================================
// LED SEQUENCE FUNCTIONS
// ==========================================================================

/**
 * @brief Update physical LEDs based on current_led_state and blinking
 * @param none
 * @return none
 */
void update_leds(void) {
    // Turn off all LEDs first
    for (int i = 0; i < 6; i++) {
        GPIOB->BSRR = (1UL << LED_PINS[i]) << 16;  // Turn OFF all LEDs
    }
    
    // Then turn on LEDs that should be on (considering blinking)
    for (int i = 0; i < 6; i++) {
        if (current_led_state & (1 << i)) {
            // For LEDs with blinking mode > 0, they are controlled by handle_blinking()
            if (blink_mode[i] > 0) {
                // This LED is controlled by blinking function - skip normal control
                continue;
            }
            // Turn ON this LED using BSRR set bits (bits 0-15) - Constant ON mode
            GPIOB->BSRR = (1UL << LED_PINS[i]);
        }
    }
}

/**
 * @brief Handle LED blinking for all LEDs that have blinking enabled
 * @param none
 * @return none
 */
void handle_blinking(void) {
    uint32_t current_time = get_current_time();
    
    for (int i = 0; i < 6; i++) {
        // Only handle LEDs that are ON and have blinking enabled
        if ((current_led_state & (1 << i)) && (blink_mode[i] > 0)) {
            // Check if it's time to toggle this blinking LED
            if (current_time - last_blink_time[i] >= blink_intervals[blink_mode[i]]) {
                last_blink_time[i] = current_time;
                blink_state[i] = !blink_state[i];  // Toggle blink state for this LED
                
                if (blink_state[i]) {
                    GPIOB->BSRR = (1UL << LED_PINS[i]);  // Turn ON this LED
                } else {
                    GPIOB->BSRR = (1UL << LED_PINS[i]) << 16;  // Turn OFF this LED
                }
            }
        }
    }
}

/**
 * @brief Get the last activated LED index in the sequence
 * @param none
 * @return Index of the last activated LED (0-5)
 */
uint8_t get_last_activated_led(void) {
    // Find the highest index LED that is currently ON
    for (int i = 5; i >= 0; i--) {
        if (current_led_state & (1 << i)) {
            return i;  // Return the highest index LED that is ON
        }
    }
    return 0;  // Default to first LED if none are ON
}

/**
 * @brief Advance to the next step in LED sequence
 * @param none
 * @return none
 */
void next_led_sequence(void) {
    led_sequence_step++;
    
    if (led_sequence_step > 6) {
        // After step 6, reset to step 0 (all off)
        led_sequence_step = 0;
        current_led_state = 0;  // Turn all LEDs off
        
        // Reset all blinking modes when all LEDs are off
        for (int i = 0; i < 6; i++) {
            blink_mode[i] = 0;
        }
    } else {
        // Turn on the current LED in sequence
        // Each step turns on one additional LED
        current_led_state = (1 << led_sequence_step) - 1;
        
        // New LEDs start with Constant ON mode (no blinking)
        // Existing LEDs keep their current blinking mode
    }
    
    update_leds();
}

/**
 * @brief Cycle to the next blinking frequency for the last activated LED
 * @param none
 * @return none
 */
void next_blink_mode(void) {
    // Only allow blinking if at least one LED is ON
    if (current_led_state == 0) {
        return;
    }
    
    // Get the last activated LED
    uint8_t last_led = get_last_activated_led();
    
    // Cycle the blinking mode for this specific LED
    blink_mode[last_led]++;
    if (blink_mode[last_led] > 3) {
        blink_mode[last_led] = 0;  // Cycle back to Constant ON
    }
    
    // Reset blinking timer for this LED
    last_blink_time[last_led] = get_current_time();
    blink_state[last_led] = 1;  // Start with LED ON
    
    update_leds();
}

// ==========================================================================
// BUTTON HANDLING FUNCTIONS
// ==========================================================================

/**
 * @brief Check if button is pressed with debouncing
 * @param port: GPIO port where button is connected
 * @param pin: Pin number of the button
 * @param last_press_time: Pointer to variable storing last press time
 * @return 1 if button is pressed, 0 otherwise
 */
uint8_t is_button_pressed(GPIO_TypeDef* port, uint16_t pin, uint32_t* last_press_time) {
    uint32_t current_time = get_current_time();
    
    // Check if button pin is HIGH (pressed) and debounce time has passed
    if (READ_BIT(port->IDR, (1UL << pin)) && 
        (current_time - *last_press_time > DEBOUNCE_DELAY)) {
        *last_press_time = current_time;
        return 1;
    }
    
    return 0;
}

// ==========================================================================
// MAIN APPLICATION
// ==========================================================================

/**
 * @brief Main function - entry point of the application
 * @param none
 * @return int
 */
int main(void) {
    // Initialize all GPIO pins (LEDs and buttons)
    GPIO_Ini();
    
    // Initialize system tick timer for timing functions
    SystemTick_Init();
    
    // Initial LED state - all OFF
    current_led_state = 0;
    
    // Initialize all blinking modes to Constant ON
    for (int i = 0; i < 6; i++) {
        blink_mode[i] = 0;
    }
    
    update_leds();
    
    // Main program loop
    while (1) {
        // ==================================================================
        // CHECK BUTTON 1 (PB4) - LED SEQUENCE CONTROL
        // ==================================================================
        if (is_button_pressed(GPIOB, 4, &last_button1_time)) {
            next_led_sequence();
        }
        
        // ==================================================================
        // CHECK BUTTON 2 (PB5) - BLINKING FREQUENCY CONTROL  
        // ==================================================================
        if (is_button_pressed(GPIOB, 5, &last_button2_time)) {
            next_blink_mode();
        }
        
        // ==================================================================
        // HANDLE BLINKING FOR ALL LEDs
        // ==================================================================
        handle_blinking();
        
        // ==================================================================
        // ADD SMALL DELAY TO REDUCE CPU USAGE
        // ==================================================================
        // Simple delay - in real application you might use timer instead
        for (volatile int i = 0; i < 1000; i++);
    }
}