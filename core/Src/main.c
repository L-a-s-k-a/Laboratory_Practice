#include <init.h>
#include <it_handlers.h>

// Array defining the blink periods corresponding to 0.2Hz, 0.8Hz, and 1.3Hz blinking frequencies (in milliseconds)
const uint32_t blink_periods[] = {5000, 1250, 769};

// Index of the currently lit LED
volatile uint8_t current_led = 0;

// Blinking mode: 1 = blinking, 0 = always on
volatile uint8_t work_mode = 1;

// Index of the blinking frequency (0 = 0.2Hz, 1 = 0.8Hz, 2 = 1.3Hz)
uint8_t blink_frequency = 0;

// Button 2 press duration counter
volatile uint32_t button2_timer = 0;

// SysTick counter
volatile uint32_t systick_counter = 0;

// Flags for buttons 1 and 2
uint8_t flagbutton1, flagbutton2;

// Button 2 state: 0 = not pressed, 1 = pressing, 2 = long press processed
uint8_t button2_state = 0;

// Array of LED pin definitions
const uint32_t led_pins[] = {GPIO_ODR_OD14, GPIO_ODR_OD7, GPIO_ODR_OD0, GPIO_ODR_OD2, GPIO_ODR_OD6, GPIO_ODR_OD1};

int main(void) {
    // Initialize GPIO, EXTI, and SysTick
    GPIO_Ini(); // GPIO initialization
    RCC_Ini(); // RCC (clock system) initialization
    EXTI_ITR_Ini(); // Interrupt controller initialization
    SysTick_Init(); // SysTick timer initialization

    while (1) {
        // Polling button 2 state
        flagbutton1 = READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) != 0;
        flagbutton2 = READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0;

        if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0) { // Button pressed
            if (button2_state == 0) { // Detected first press
                button2_state = 1;  // Switch to pressing state
                button2_timer = systick_counter; // Record the press time
            } else if (button2_state == 1) { // Pressing state
                uint32_t press_duration = systick_counter - button2_timer;
                if (press_duration >= 2000) { // Long press detected
                    work_mode ^= 1;        // Toggle work mode
                    button2_state = 2;     // Mark long press as processed
                }
            }
        } else { // Button released
            if (button2_state == 1) { // Released after short press
                blink_frequency = (blink_frequency + 1) % 3; // Change frequency
            }
            // Reset button state
            button2_state = 0;
        }

        // LED control logic
        static uint32_t blink_counter = 0;
        if (work_mode == 0) { // Blinking mode
            if (systick_counter - blink_counter >= blink_periods[blink_frequency]) {
                blink_counter = systick_counter;
                GPIOB->ODR ^= led_pins[current_led]; // Toggle the current LED
            }
        } else { // Always-on mode
            if (current_led == 2) {
                continue; // Skip if current LED is index 2
            }
            update_led_state(); // Light up the current LED
        }
    }
}
