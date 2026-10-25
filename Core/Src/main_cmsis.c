#include "stm32f4xx.h"

/*
 * Variant 8, main task - CMSIS
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

#define DEBOUNCE_TICKS      100000UL
#define LEDS_COUNT          6U

/* Variables for STMViewer */
volatile uint8_t flag     = 0;
volatile uint8_t last_btn = 0;
volatile uint8_t leds_on  = 0;
volatile uint8_t swapped  = 0;

static uint8_t btn_idle[3] = {0, 0, 0};

static void delay(volatile uint32_t ticks)
{
    while (ticks--) { }
}

static void GPIO_Ini(void)
{
    /* Clocks for GPIOA, GPIOB, GPIOC */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    (void)READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

    /* LED1: PB0 */
    SET_BIT(GPIOB->MODER,    GPIO_MODER_MODE0_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);
    SET_BIT(GPIOB->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR0_0);
    CLEAR_BIT(GPIOB->PUPDR,  GPIO_PUPDR_PUPDR0);

    /* LED2: PB7 */
    SET_BIT(GPIOB->MODER,    GPIO_MODER_MODE7_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);
    SET_BIT(GPIOB->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR7_0);
    CLEAR_BIT(GPIOB->PUPDR,  GPIO_PUPDR_PUPDR7);

    /* LED3: PB14 */
    SET_BIT(GPIOB->MODER,    GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    SET_BIT(GPIOB->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR14_0);
    CLEAR_BIT(GPIOB->PUPDR,  GPIO_PUPDR_PUPDR14);

    /* LED4: PA5 (D13) */
    SET_BIT(GPIOA->MODER,    GPIO_MODER_MODE5_0);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_5);
    SET_BIT(GPIOA->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR5_0);
    CLEAR_BIT(GPIOA->PUPDR,  GPIO_PUPDR_PUPDR5);

    /* LED5: PA6 (D12) */
    SET_BIT(GPIOA->MODER,    GPIO_MODER_MODE6_0);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_6);
    SET_BIT(GPIOA->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR6_0);
    CLEAR_BIT(GPIOA->PUPDR,  GPIO_PUPDR_PUPDR6);

    /* LED6: PA7 (D11) */
    SET_BIT(GPIOA->MODER,    GPIO_MODER_MODE7_0);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_7);
    SET_BIT(GPIOA->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR7_0);
    CLEAR_BIT(GPIOA->PUPDR,  GPIO_PUPDR_PUPDR7);

    /* Button 1: PC13, input (pull-down is on the board) */

    /* Button 2: PA3 (A0), input, pull-down */
    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR3_1);

    /* Button 3: PC0 (A1), input, pull-down */
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0_1);
}

/* Turn on the first "n" LEDs, turn off the rest */
static void Leds_Set(uint8_t n)
{
    if (n >= 1) { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS0); }
    else        { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR0); }

    if (n >= 2) { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS7); }
    else        { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR7); }

    if (n >= 3) { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS14); }
    else        { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR14); }

    if (n >= 4) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5); }
    else        { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR5); }

    if (n >= 5) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS6); }
    else        { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR6); }

    if (n >= 6) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS7); }
    else        { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR7); }
}

/* Raw level of button n (0..2) */
static uint8_t Btn_Raw(uint8_t n)
{
    if (n == 0) { return (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0) ? 1 : 0; }
    if (n == 1) { return (READ_BIT(GPIOA->IDR, GPIO_IDR_IDR_3)  != 0) ? 1 : 0; }
    return (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0) != 0) ? 1 : 0;
}

static uint8_t Btn_Pressed(uint8_t n)
{
    return (Btn_Raw(n) != btn_idle[n]) ? 1 : 0;
}

/* Returns 1 once per confirmed press (debounce, waits for release) */
static uint8_t Btn_Event(uint8_t n)
{
    if (!Btn_Pressed(n)) { return 0; }

    flag = 1;
    delay(DEBOUNCE_TICKS);
    if (!Btn_Pressed(n)) { flag = 0; return 0; }

    last_btn = (uint8_t)(n + 1);
    while (Btn_Pressed(n)) { }
    delay(DEBOUNCE_TICKS);
    flag = 0;
    return 1;
}

static void Led_Add(void)
{
    if (leds_on < LEDS_COUNT) { leds_on++; }
}

static void Led_Remove(void)
{
    if (leds_on > 0) { leds_on--; }
}

int main(void)
{
    GPIO_Ini();
    Leds_Set(0);

    delay(DEBOUNCE_TICKS);
    btn_idle[0] = 0;
    btn_idle[1] = Btn_Raw(1);
    btn_idle[2] = Btn_Raw(2);

    while (1)
    {
        if (Btn_Event(0))            /* button 1 */
        {
            if (swapped == 0) { Led_Add(); }
            else              { Led_Remove(); }
            Leds_Set(leds_on);
        }

        if (Btn_Event(1))            /* button 2 */
        {
            if (swapped == 0) { Led_Remove(); }
            else              { Led_Add(); }
            Leds_Set(leds_on);
        }

        if (Btn_Event(2))            /* button 3: swap functions */
        {
            swapped = (uint8_t)(swapped ^ 1U);
        }
    }
}
