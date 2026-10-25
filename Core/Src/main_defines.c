#include <stdint.h>

/*
 * Variant 8, main task - CUSTOM #define AND MACROS (no CMSIS)
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

/* ---- Registers ---- */
#define RCC_AHB1ENR     (*(volatile uint32_t*)(0x40023800UL + 0x30UL))

#define GPIOA_MODER     (*(volatile uint32_t*)(0x40020000UL + 0x00UL))
#define GPIOA_PUPDR     (*(volatile uint32_t*)(0x40020000UL + 0x0CUL))
#define GPIOA_IDR       (*(volatile uint32_t*)(0x40020000UL + 0x10UL))
#define GPIOA_BSRR      (*(volatile uint32_t*)(0x40020000UL + 0x18UL))

#define GPIOB_MODER     (*(volatile uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_BSRR      (*(volatile uint32_t*)(0x40020400UL + 0x18UL))

#define GPIOC_PUPDR     (*(volatile uint32_t*)(0x40020800UL + 0x0CUL))
#define GPIOC_IDR       (*(volatile uint32_t*)(0x40020800UL + 0x10UL))

/* ---- Values ---- */
#define CLOCK_GPIOABC       0x00000007UL   /* GPIOA, GPIOB, GPIOC clock on */

#define GPIOB_MODE_OUT      0x10004001UL   /* PB0, PB7, PB14 output */
#define GPIOA_MODE_OUT      0x00005400UL   /* PA5, PA6, PA7 output */

#define PA3_PULLDOWN        0x00000080UL   /* button 2 */
#define PC0_PULLDOWN        0x00000002UL   /* button 3 */

#define BTN1_PIN            0x00002000UL   /* PC13 */
#define BTN2_PIN            0x00000008UL   /* PA3  */
#define BTN3_PIN            0x00000001UL   /* PC0  */

#define LED1_SET            0x00000001UL   /* PB0  */
#define LED1_RESET          0x00010000UL
#define LED2_SET            0x00000080UL   /* PB7  */
#define LED2_RESET          0x00800000UL
#define LED3_SET            0x00004000UL   /* PB14 */
#define LED3_RESET          0x40000000UL
#define LED4_SET            0x00000020UL   /* PA5  */
#define LED4_RESET          0x00200000UL
#define LED5_SET            0x00000040UL   /* PA6  */
#define LED5_RESET          0x00400000UL
#define LED6_SET            0x00000080UL   /* PA7  */
#define LED6_RESET          0x00800000UL

#define DEBOUNCE_TICKS      100000UL
#define LEDS_COUNT          6U

/* ---- Macros ---- */
#define SET_BITS(REG, MASK)     ((REG) |= (MASK))
#define WRITE_BITS(REG, VAL)    ((REG) = (VAL))
#define READ_BITS(REG, MASK)    ((REG) & (MASK))

/* Variables for STMViewer */
volatile uint8_t flag     = 0;
volatile uint8_t last_btn = 0;
volatile uint8_t leds_on  = 0;
volatile uint8_t swapped  = 0;

static uint8_t btn_idle[3] = {0, 0, 0};

static void delay(volatile uint32_t t)
{
    while (t--) { }
}

static void GPIO_Ini(void)
{
    SET_BITS(RCC_AHB1ENR, CLOCK_GPIOABC);
    SET_BITS(GPIOB_MODER, GPIOB_MODE_OUT);
    SET_BITS(GPIOA_MODER, GPIOA_MODE_OUT);
    SET_BITS(GPIOA_PUPDR, PA3_PULLDOWN);
    SET_BITS(GPIOC_PUPDR, PC0_PULLDOWN);
    /* PC13 stays input (the board has a pull-down) */
}

/* Turn on first n LEDs, turn off the rest */
static void Leds_Set(uint8_t n)
{
    WRITE_BITS(GPIOB_BSRR, (n >= 1) ? LED1_SET : LED1_RESET);
    WRITE_BITS(GPIOB_BSRR, (n >= 2) ? LED2_SET : LED2_RESET);
    WRITE_BITS(GPIOB_BSRR, (n >= 3) ? LED3_SET : LED3_RESET);
    WRITE_BITS(GPIOA_BSRR, (n >= 4) ? LED4_SET : LED4_RESET);
    WRITE_BITS(GPIOA_BSRR, (n >= 5) ? LED5_SET : LED5_RESET);
    WRITE_BITS(GPIOA_BSRR, (n >= 6) ? LED6_SET : LED6_RESET);
}

/* Raw level of button n (0..2) */
static uint8_t Btn_Raw(uint8_t n)
{
    if (n == 0) { return (READ_BITS(GPIOC_IDR, BTN1_PIN) != 0) ? 1 : 0; }
    if (n == 1) { return (READ_BITS(GPIOA_IDR, BTN2_PIN) != 0) ? 1 : 0; }
    return (READ_BITS(GPIOC_IDR, BTN3_PIN) != 0) ? 1 : 0;
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
