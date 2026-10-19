#include "stm32f4xx.h"

/*
 * Variant 10, extra task: blinking LEDs without hardware timers
 *   LED1 - PB0  (green, board)  - direct memory access
 *   LED2 - PB7  (blue, board)   - custom defines
 *   LED3 - PB14 (red, board)    - CMSIS
 *   LED4 - PA5  (D13, external) - CMSIS
 *   LED5 - PA6  (D12, external) - CMSIS
 *   LED6 - PA7  (D11, external) - CMSIS
 *   Button 1 - PC13 (board B1): blinking frequency UP   (3 levels)
 *   Button 2 - PA3  (A0, HW-483): blinking frequency DOWN (3 levels)
 *   Button 3 - PC0  (A1):
 *       short press - mode ON: turns on the next LED (up to 6)
 *                     mode OFF: turns off one LED
 *       hold        - switches the mode ON <-> OFF (once per hold)
 *
 * Time is counted in main loop ticks (about 1 ms each, busy-wait delay).
 * If blinking is too fast or too slow, change TICK_LOOPS.
 * Do not hold the buttons while powering the board.
 */

/* ---- Custom defines for LED2 (PB7) ---- */
#define MY_GPIOB_MODER          (*(volatile uint32_t*)(0x40020400UL + 0x00UL))
#define MY_GPIOB_OTYPER         (*(volatile uint32_t*)(0x40020400UL + 0x04UL))
#define MY_GPIOB_OSPEEDR        (*(volatile uint32_t*)(0x40020400UL + 0x08UL))
#define MY_GPIOB_PUPDR          (*(volatile uint32_t*)(0x40020400UL + 0x0CUL))
#define MY_GPIOB_BSRR           (*(volatile uint32_t*)(0x40020400UL + 0x18UL))

#define MY_PIN7_MODE_OUT        0x00004000UL
#define MY_PIN7_OTYPE_PP_MASK   0xFFFFFF7FUL
#define MY_PIN7_OSPEED_MID      0x00004000UL
#define MY_PIN7_PUPDR_NONE_MASK 0xFFFF3FFFUL
#define MY_PIN7_SET             0x00000080UL
#define MY_PIN7_RESET           0x00800000UL

#define MY_SET_BITS(REG, MASK)  ((REG) |= (MASK))
#define MY_CLR_BITS(REG, MASK)  ((REG) &= (MASK))

/* ---- Settings ---- */
#define LEDS_COUNT          6U
#define TICK_LOOPS          2000UL    /* one tick, about 1 ms at 16 MHz */
#define DEBOUNCE_TICKS      20U       /* press is accepted after 20 ticks */
#define HOLD_START_TICKS    600U      /* hold button 3 this long to switch the mode */

/* Half-period of blinking for levels 1, 2, 3 (in ticks): about 1, 2, 4 Hz */
static const uint16_t half_period[3] = {500U, 250U, 125U};

/* Variables for STMViewer */
volatile uint8_t flag        = 0;   /* 1 while any button is pressed */
volatile uint8_t freq_level  = 1;   /* blinking frequency level (1..3) */
volatile uint8_t leds_on     = 1;   /* how many LEDs take part in blinking */
volatile uint8_t blink_state = 1;   /* 1 - LEDs are lit now, 0 - dark */
volatile uint8_t btn3_mode   = 0;   /* 0 - button 3 adds LEDs, 1 - removes */

static uint8_t  btn_idle[3] = {0, 0, 0};
static uint16_t held[3]     = {0, 0, 0};
static uint16_t blink_cnt   = 0;

static void delay(volatile uint32_t ticks)
{
    while (ticks--) { }
}

static void GPIO_Ini(void)
{
    /* Clocks for GPIOA, GPIOB, GPIOC */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    (void)READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

    /* LED1: PB0, direct memory access */
    *(volatile uint32_t*)(0x40020400UL + 0x00UL) |= 0x00000001UL;
    *(volatile uint32_t*)(0x40020400UL + 0x04UL) &= 0xFFFFFFFEUL;
    *(volatile uint32_t*)(0x40020400UL + 0x08UL) |= 0x00000001UL;
    *(volatile uint32_t*)(0x40020400UL + 0x0CUL) &= 0xFFFFFFFCUL;

    /* LED2: PB7, custom defines */
    MY_SET_BITS(MY_GPIOB_MODER,   MY_PIN7_MODE_OUT);
    MY_CLR_BITS(MY_GPIOB_OTYPER,  MY_PIN7_OTYPE_PP_MASK);
    MY_SET_BITS(MY_GPIOB_OSPEEDR, MY_PIN7_OSPEED_MID);
    MY_CLR_BITS(MY_GPIOB_PUPDR,   MY_PIN7_PUPDR_NONE_MASK);

    /* LED3: PB14, CMSIS */
    SET_BIT(GPIOB->MODER,    GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    SET_BIT(GPIOB->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR14_0);
    CLEAR_BIT(GPIOB->PUPDR,  GPIO_PUPDR_PUPDR14);

    /* LED4: PA5 (D13), CMSIS */
    SET_BIT(GPIOA->MODER,    GPIO_MODER_MODE5_0);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_5);
    SET_BIT(GPIOA->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR5_0);
    CLEAR_BIT(GPIOA->PUPDR,  GPIO_PUPDR_PUPDR5);

    /* LED5: PA6 (D12), CMSIS */
    SET_BIT(GPIOA->MODER,    GPIO_MODER_MODE6_0);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_6);
    SET_BIT(GPIOA->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR6_0);
    CLEAR_BIT(GPIOA->PUPDR,  GPIO_PUPDR_PUPDR6);

    /* LED6: PA7 (D11), CMSIS */
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

/* Turn on the first "count" LEDs, turn off the rest */
static void Leds_Set(uint8_t count)
{
    /* LED1 - PB0, direct memory access */
    if (count >= 1) { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x00000001UL; }
    else            { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x00010000UL; }

    /* LED2 - PB7, custom defines */
    if (count >= 2) { MY_GPIOB_BSRR = MY_PIN7_SET; }
    else            { MY_GPIOB_BSRR = MY_PIN7_RESET; }

    /* LED3 - PB14, CMSIS */
    if (count >= 3) { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS14); }
    else            { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR14); }

    /* LED4 - PA5, CMSIS */
    if (count >= 4) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5); }
    else            { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR5); }

    /* LED5 - PA6, CMSIS */
    if (count >= 5) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS6); }
    else            { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR6); }

    /* LED6 - PA7, CMSIS */
    if (count >= 6) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS7); }
    else            { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR7); }
}

/* Raw level of button n (0..2), CMSIS */
static uint8_t Btn_Raw(uint8_t n)
{
    if (n == 0) { return (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0) ? 1 : 0; }
    if (n == 1) { return (READ_BIT(GPIOA->IDR, GPIO_IDR_IDR_3)  != 0) ? 1 : 0; }
    return (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0) != 0) ? 1 : 0;
}

/* Show the change at once (start of a lit phase) */
static void Show_Now(void)
{
    blink_state = 1;
    blink_cnt = 0;
}

int main(void)
{
    uint8_t  n;
    uint16_t prev_held3;

    GPIO_Ini();
    Leds_Set(0);

    /* Remember idle level of the buttons */
    delay(TICK_LOOPS * 20UL);
    btn_idle[0] = 0;
    btn_idle[1] = Btn_Raw(1);
    btn_idle[2] = Btn_Raw(2);

    while (1)
    {
        delay(TICK_LOOPS);                    /* one tick, about 1 ms */

        prev_held3 = held[2];

        /* ---- Count how long each button is held ---- */
        for (n = 0; n < 3; n++)
        {
            if (Btn_Raw(n) != btn_idle[n])
            {
                if (held[n] < 65000U) { held[n]++; }
            }
            else
            {
                held[n] = 0;
            }
        }
        flag = ((held[0] != 0) || (held[1] != 0) || (held[2] != 0)) ? 1 : 0;

        /* ---- Button 1: frequency up ---- */
        if (held[0] == DEBOUNCE_TICKS)
        {
            if (freq_level < 3) { freq_level++; }
        }

        /* ---- Button 2: frequency down ---- */
        if (held[1] == DEBOUNCE_TICKS)
        {
            if (freq_level > 1) { freq_level--; }
        }

        /* ---- Button 3: short press - one action, hold - switch the mode ---- */
        if ((held[2] == 0) && (prev_held3 >= DEBOUNCE_TICKS) && (prev_held3 < HOLD_START_TICKS))
        {
            /* released after a short press */
            if (btn3_mode == 0)
            {
                if (leds_on < LEDS_COUNT) { leds_on++; }
            }
            else
            {
                if (leds_on > 0) { leds_on--; }
            }
            Show_Now();
        }

        if (held[2] == HOLD_START_TICKS)
        {
            /* long hold: switch ON <-> OFF once, no LED changes */
            btn3_mode = (uint8_t)(btn3_mode ^ 1U);
        }

        /* -


--- Blinking without timer: count ticks ---- */
        blink_cnt++;
        if (blink_cnt >= half_period[freq_level - 1U])
        {
            blink_cnt = 0;
            blink_state = (uint8_t)(blink_state ^ 1U);
        }

        if (blink_state != 0) { Leds_Set(leds_on); }
        else                  { Leds_Set(0); }
    }
}

