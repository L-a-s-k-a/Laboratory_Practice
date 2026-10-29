#include <stdint.h>

/*
 * Variant 8, main task - DIRECT MEMORY ACCESS (no CMSIS, no defines)
 *   LED1 - PB0  (green, board)    LED4 - PA5 (D13, external)
 *   LED2 - PB7  (blue, board)     LED5 - PA6 (D12, external)
 *   LED3 - PB14 (red, board)      LED6 - PA7 (D11, external)
 *   Button 1 - PC13 (board B1)
 *   Button 2 - PA3  (A0, HW-483)
 *   Button 3 - PC0  (A1)
 *
 * Button 1: turns on LEDs one by one, stops when all are on
 * Button 2: turns off LEDs one by one, stops when all are off
 * Button 3: swaps functions of buttons 1 and 2
 * Do not hold the buttons while powering the board.
 */

volatile uint8_t flag     = 0;   // 1 while a button is pressed
volatile uint8_t last_btn = 0;   // last pressed button (1..3)
volatile uint8_t leds_on  = 0;   // how many LEDs are on (0..6)
volatile uint8_t swapped  = 0;   // 1 - buttons 1 and 2 are swapped

uint8_t btn_idle[3] = {0, 0, 0}; // idle level of each button

void delay(volatile uint32_t t)
{
    while (t--) { }
}

// Turn on first n LEDs, turn off the rest
void leds_set(uint8_t n)
{
    // GPIOB BSRR: PB0, PB7, PB14
    if (n >= 1) { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x00000001UL; }
    else        { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x00010000UL; }

    if (n >= 2) { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x00000080UL; }
    else        { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x00800000UL; }

    if (n >= 3) { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x00004000UL; }
    else        { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x40000000UL; }

    // GPIOA BSRR: PA5, PA6, PA7
    if (n >= 4) { *(volatile uint32_t*)(0x40020000UL + 0x18UL) = 0x00000020UL; }
    else        { *(volatile uint32_t*)(0x40020000UL + 0x18UL) = 0x00200000UL; }

    if (n >= 5) { *(volatile uint32_t*)(0x40020000UL + 0x18UL) = 0x00000040UL; }
    else        { *(volatile uint32_t*)(0x40020000UL + 0x18UL) = 0x00400000UL; }

    if (n >= 6) { *(volatile uint32_t*)(0x40020000UL + 0x18UL) = 0x00000080UL; }
    else        { *(volatile uint32_t*)(0x40020000UL + 0x18UL) = 0x00800000UL; }
}

// Raw level of button n (0..2)
uint8_t btn_raw(uint8_t n)
{
    // GPIOC IDR PC13
    if (n == 0) { return (*(volatile uint32_t*)(0x40020800UL + 0x10UL) & 0x2000UL) ? 1 : 0; }
    // GPIOA IDR PA3
    if (n == 1) { return (*(volatile uint32_t*)(0x40020000UL + 0x10UL) & 0x0008UL) ? 1 : 0; }
    // GPIOC IDR PC0
    return (*(volatile uint32_t*)(0x40020800UL + 0x10UL) & 0x0001UL) ? 1 : 0;
}

// 1 if button n differs from its idle level
uint8_t btn_pressed(uint8_t n)
{
    return (btn_raw(n) != btn_idle[n]) ? 1 : 0;
}

// Returns 1 once per confirmed press (debounce, waits for release)
uint8_t btn_event(uint8_t n)
{
    if (!btn_pressed(n)) { return 0; }

    flag = 1;
    delay(100000UL);                       // debounce on press
    if (!btn_pressed(n)) { flag = 0; return 0; }

    last_btn = n + 1;
    while (btn_pressed(n)) { }             // wait for release
    delay(100000UL);                       // debounce on release
    flag = 0;
    return 1;
}

void led_add(void)
{
    if (leds_on < 6) { leds_on++; }
}

void led_remove(void)
{
    if (leds_on > 0) { leds_on--; }
}

int main(void)
{
    // RCC AHB1ENR: clock for GPIOA, GPIOB, GPIOC
    *(volatile uint32_t*)(0x40023800UL + 0x30UL) |= 0x07UL;

    // GPIOB MODER: PB0, PB7, PB14 as outputs
    *(volatile uint32_t*)(0x40020400UL + 0x00UL) |= 0x10004001UL;
    // GPIOA MODER: PA5, PA6, PA7 as outputs
    *(volatile uint32_t*)(0x40020000UL + 0x00UL) |= 0x00005400UL;

    // GPIOA PUPDR: PA3 pull-down (button 2)
    *(volatile uint32_t*)(0x40020000UL + 0x0CUL) |= 0x00000080UL;
    // GPIOC PUPDR: PC0 pull-down (button 3)
    *(volatile uint32_t*)(0x40020800UL + 0x0CUL) |= 0x00000002UL;
    // PC13 stays input, the board has a pull-down

    leds_set(0);

    delay(100000UL);                       // let inputs settle
    btn_idle[0] = 0;
    btn_idle[1] = btn_raw(1);
    btn_idle[2] = btn_raw(2);

    while (1)
    {
        if (btn_event(0))                  // button 1
        {
            if (swapped == 0) { led_add(); }
            else              { led_remove(); }
            leds_set(leds_on);
        }

        if (btn_event(1))                  // button 2
        {
            if (swapped == 0) { led_remove(); }
            else              { led_add(); }
            leds_set(leds_on);
        }

        if (btn_event(2))                  // button 3: swap functions
        {
            swapped = swapped ^ 1;
        }
    }
}
