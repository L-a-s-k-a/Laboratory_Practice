/*
 * HUB12 signals:
 *   OE=PF12 (ACTIVE-HIGH on this panel), A=PF14, B=PF15,
 *   S(CLOCK)=PF3, L(LATCH)=PF5, R(DATA)=PF10 (data LOW = pixel on)
 *
 * Joystick: VRx=PA3/A0, VRy=PC0/A1, SW=PE11/D5, powered from 3V3.
 */

/* CMSIS device header: provides STM32F7 register definitions and bit masks. */
#include "stm32f7xx.h"

/* Panel dimensions, scan layout, and number of data planes used to refresh it. */
#define MATRIX_WIDTH        32U
#define MATRIX_HEIGHT       16U
#define MATRIX_SCAN_ROWS    4U
#define DATA_PLANES         4U
#define PANEL_SECTIONS      4U

/* GPIOF pin assignments for the HUB12 panel control and data signals. */
#define HUB12_OE_PIN        12U
#define HUB12_A_PIN         14U
#define HUB12_B_PIN         15U
#define HUB12_DATA_PIN      10U
#define HUB12_LATCH_PIN     5U
#define HUB12_CLOCK_PIN     3U

/* Joystick analog pin/channel assignments and digital switch pin. */
#define JOYSTICK_X_PIN      3U
#define JOYSTICK_Y_PIN      0U
#define JOYSTICK_X_CHANNEL  3U
#define JOYSTICK_Y_CHANNEL  10U
#define JOYSTICK_SW_PIN     11U

/* Game sizing, ADC center/dead-zone, initial snake length, and movement interval. */
#define CELL_COUNT          (MATRIX_WIDTH * MATRIX_HEIGHT)
#define SNAKE_CAPACITY      CELL_COUNT
#define ADC_CENTER          2048U
#define ADC_DEAD_ZONE       850U
#define SNAKE_MOVE_MS       115U
#define START_LENGTH        3U

/* Brightness: raise for brighter, lower if you see flicker. */
#define ROW_ON_CYCLES       4000U

/* 0 = solid border, 1 = dotted border (easier to see a snake running
 * right next to the wall, because it doesn't merge with it). */
#define BORDER_DOTTED       0

/* OE is active-high on this panel: one place to change if that ever flips. */
#define OE_ON()   (GPIOF->BSRR = 1U << HUB12_OE_PIN)
#define OE_OFF()  (GPIOF->BSRR = 1U << (HUB12_OE_PIN + 16U))

/* Difficulty selection: EASY wraps at edges; HARD treats the border as a collision. */
typedef enum
{
    MODE_EASY = 0,
    MODE_HARD = 1
} game_mode_t;

/* One cell coordinate on the matrix, shared by snake segments and food. */
typedef struct
{
    uint8_t x;
    uint8_t y;
} cell_t;

/* Persistent game state: pixels, ordered snake body, food, direction, mode, clock, and random seed. */
static uint8_t framebuffer[CELL_COUNT];
static cell_t snake[SNAKE_CAPACITY];
static uint16_t snake_length;
static cell_t food;
static int8_t direction_x;
static int8_t direction_y;
static game_mode_t game_mode = MODE_EASY;
static uint32_t random_state = 0x13579BDFU;
static volatile uint32_t system_millis;
static const uint8_t address_order[MATRIX_SCAN_ROWS] = {0U, 3U, 1U, 2U};

static const uint8_t visual_row_by_plane_and_scan[DATA_PLANES][MATRIX_SCAN_ROWS] =
{
    {12U, 15U, 13U, 14U},
    {8U,  11U, 9U, 10U},
    {4U,  7U, 5U, 6U},
    {0U,  3U, 1U, 2U}
};

/* 5x5 digits, stored column by column: digit_patterns[d][column],
 * bit0 = top row. Digit 0 fixed: it had its top corners missing. */
static const uint8_t digit_patterns[10][5] =
{
    {0x1FU, 0x11U, 0x11U, 0x11U, 0x1FU},
    {0x00U, 0x12U, 0x1FU, 0x10U, 0x00U},
    {0x1DU, 0x15U, 0x15U, 0x15U, 0x17U},
    {0x11U, 0x15U, 0x15U, 0x15U, 0x1FU},
    {0x07U, 0x04U, 0x04U, 0x04U, 0x1FU},
    {0x17U, 0x15U, 0x15U, 0x15U, 0x1DU},
    {0x1FU, 0x15U, 0x15U, 0x15U, 0x1DU},
    {0x01U, 0x01U, 0x01U, 0x01U, 0x1FU},
    {0x1FU, 0x15U, 0x15U, 0x15U, 0x1FU},
    {0x17U, 0x15U, 0x15U, 0x15U, 0x1FU}
};

static void delay_cycles(volatile uint32_t cycles)
{
    while (cycles-- != 0U)
    {
        __NOP();
    }
}

/* Interrupt handler called by SysTick every millisecond; advances the software clock. */
void SysTick_Handler(void)
{
    system_millis++;
}

/* Configure SysTick to trigger an interrupt every 1 ms. */
static void systick_init(void)
{
    SysTick->LOAD = (SystemCoreClock / 1000U) - 1U;
    SysTick->VAL = 0U;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
}

/* Read the millisecond counter used for game and screen timing. */
static uint32_t millis(void)
{
    return system_millis;
}

/* Enable GPIOF and configure HUB12 signal pins as outputs with safe initial levels. */
static void matrix_gpio_init(void)
{
    /* Enable the GPIOF peripheral clock before configuring its pins. */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;

    GPIOF->MODER &= ~((3U << (HUB12_OE_PIN * 2U)) |
                      (3U << (HUB12_A_PIN * 2U)) |
                      (3U << (HUB12_B_PIN * 2U)) |
                      (3U << (HUB12_DATA_PIN * 2U)) |
                      (3U << (HUB12_LATCH_PIN * 2U)) |
                      (3U << (HUB12_CLOCK_PIN * 2U)));
    GPIOF->MODER |= ((1U << (HUB12_OE_PIN * 2U)) |
                     (1U << (HUB12_A_PIN * 2U)) |
                     (1U << (HUB12_B_PIN * 2U)) |
                     (1U << (HUB12_DATA_PIN * 2U)) |
                     (1U << (HUB12_LATCH_PIN * 2U)) |
                     (1U << (HUB12_CLOCK_PIN * 2U)));
    GPIOF->OTYPER &= ~((1U << HUB12_OE_PIN) |
                       (1U << HUB12_A_PIN) |
                       (1U << HUB12_B_PIN) |
                       (1U << HUB12_DATA_PIN) |
                       (1U << HUB12_LATCH_PIN) |
                       (1U << HUB12_CLOCK_PIN));

    /* Start blanked: OE low (off), data/clock/latch idle high. */
    GPIOF->BSRR = (1U << (HUB12_OE_PIN + 16U)) |
                  (1U << HUB12_DATA_PIN) |
                  (1U << HUB12_CLOCK_PIN) |
                  (1U << HUB12_LATCH_PIN);
}

/* Enable GPIO and ADC clocks; configure analog joystick axes and its active-low button. */
static void joystick_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN |
                    RCC_AHB1ENR_GPIOCEN |
                    RCC_AHB1ENR_GPIOEEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    GPIOA->MODER |= 3U << (JOYSTICK_X_PIN * 2U);
    GPIOC->MODER |= 3U << (JOYSTICK_Y_PIN * 2U);

    GPIOE->MODER &= ~(3U << (JOYSTICK_SW_PIN * 2U));
    GPIOE->PUPDR &= ~(3U << (JOYSTICK_SW_PIN * 2U));
    GPIOE->PUPDR |= 1U << (JOYSTICK_SW_PIN * 2U);

    /* Reset ADC configuration, set conversion sampling times, then enable ADC1. */
    ADC1->CR2 = 0U;
    ADC1->CR1 = 0U;
    ADC1->SMPR2 = 7U << (JOYSTICK_X_CHANNEL * 3U);
    ADC1->SMPR1 = 7U << ((JOYSTICK_Y_CHANNEL - 10U) * 3U);
    ADC1->CR2 = ADC_CR2_ADON;
}

/* Sample an ADC channel four times and average the readings to reduce noise. */
static uint16_t joystick_read(uint8_t channel)
{
    uint32_t total = 0U;

    /* Four conversions are averaged to make the joystick reading less noisy. */
    for (uint8_t sample = 0U; sample < 4U; sample++)
    {
        ADC1->SR = 0U;
        ADC1->SQR1 = 0U;
        ADC1->SQR3 = channel;
        ADC1->CR2 |= ADC_CR2_SWSTART;

        while ((ADC1->SR & ADC_SR_EOC) == 0U)
        {
        }

        total += ADC1->DR;
    }

    return (uint16_t)(total / 4U);
}

/* Return nonzero when the joystick switch input is low (button pressed). */
static uint8_t joystick_button_pressed(void)
{
    return (GPIOE->IDR & (1U << JOYSTICK_SW_PIN)) == 0U;
}

/* ---- Framebuffer ---- */

/* Update one valid pixel in the row-major software framebuffer. */
static void matrix_set_pixel(uint8_t x, uint8_t y, uint8_t on)
{
    if ((x < MATRIX_WIDTH) && (y < MATRIX_HEIGHT))
    {
        framebuffer[(uint32_t)y * MATRIX_WIDTH + x] = on;
    }
}

/* Clear the framebuffer so the next screen/frame can be drawn from scratch. */
static void matrix_clear(void)
{
    for (uint32_t i = 0U; i < CELL_COUNT; i++)
    {
        framebuffer[i] = 0U;
    }
}

/* ---- 3x5 text: each row is 3 bits, bit2 = left pixel ---- */

/* Look up the 3x5 bitmap for a supported uppercase letter. */
static const uint8_t *glyph(char c)
{
    static const uint8_t gW[5] = {5, 5, 5, 7, 5};
    static const uint8_t gE[5] = {7, 4, 6, 4, 7};
    static const uint8_t gL[5] = {4, 4, 4, 4, 7};
    static const uint8_t gC[5] = {7, 4, 4, 4, 7};
    static const uint8_t gO[5] = {7, 5, 5, 5, 7};
    static const uint8_t gM[5] = {5, 7, 7, 5, 5};
    static const uint8_t gG[5] = {7, 4, 5, 5, 7};
    static const uint8_t gA[5] = {2, 5, 7, 5, 5};
    static const uint8_t gV[5] = {5, 5, 5, 5, 2};
    static const uint8_t gR[5] = {6, 5, 6, 5, 5};
    static const uint8_t gS[5] = {7, 4, 7, 1, 7};
    static const uint8_t gY[5] = {5, 5, 2, 2, 2};
    static const uint8_t gH[5] = {5, 5, 7, 5, 5};
    static const uint8_t gD[5] = {6, 5, 5, 5, 6};

    /* Select the bitmap corresponding to this character. */
    switch (c)
    {
        case 'W': return gW;
        case 'E': return gE;
        case 'L': return gL;
        case 'C': return gC;
        case 'O': return gO;
        case 'M': return gM;
        case 'G': return gG;
        case 'A': return gA;
        case 'V': return gV;
        case 'R': return gR;
        case 'S': return gS;
        case 'Y': return gY;
        case 'H': return gH;
        case 'D': return gD;
        default:  return 0;
    }
}

/* Render a string by plotting each letter bitmap with a one-column gap. */
static void draw_text(uint8_t x, uint8_t y, const char *text)
{
    while (*text != '\0')
    {
        const uint8_t *g = glyph(*text++);

        if (g != 0)
        {
            for (uint8_t row = 0U; row < 5U; row++)
            {
                for (uint8_t col = 0U; col < 3U; col++)
                {
                    if ((g[row] & (4U >> col)) != 0U)
                    {
                        matrix_set_pixel((uint8_t)(x + col),
                                         (uint8_t)(y + row), 1U);
                    }
                }
            }
        }
        x += 4U;   /* 3 px glyph + 1 px gap */
    }
}

/* Draw one 5x5 digit with its top-left corner at (x0, y0). No clear. */
/* Draw one 5x5 digit at the requested top-left position without clearing the screen. */
static void draw_digit_at(uint8_t x0, uint8_t y0, uint8_t digit)
{
    if (digit < 10U)
    {
        for (uint8_t x = 0U; x < 5U; x++)
        {
            for (uint8_t y = 0U; y < 5U; y++)
            {
                if ((digit_patterns[digit][x] & (1U << y)) != 0U)
                {
                    matrix_set_pixel((uint8_t)(x0 + x),
                                     (uint8_t)(y0 + y), 1U);
                }
            }
        }
    }
}

/* Draw a 1 to 3 digit number horizontally centered, top at row y. */
/* Center a one-, two-, or three-digit value horizontally at the requested row. */
static void draw_number(uint8_t y, uint16_t value)
{
    if (value > 999U)
    {
        value = 999U;
    }

    uint8_t digits = (value >= 100U) ? 3U : ((value >= 10U) ? 2U : 1U);
    uint16_t divisor = (digits == 3U) ? 100U : ((digits == 2U) ? 10U : 1U);
    uint8_t x = (uint8_t)((MATRIX_WIDTH - (digits * 6U - 1U)) / 2U);

    while (divisor != 0U)
    {
        draw_digit_at(x, y, (uint8_t)((value / divisor) % 10U));
        x += 6U;   /* 5 px digit + 1 px gap */
        divisor /= 10U;
    }
}

/* Clear the screen and draw a large countdown digit in the center. */
static void draw_digit(uint8_t digit)
{
    matrix_clear();
    draw_digit_at(14U, 5U, digit);
}

/* ---- Display refresh ----
 * Per scan row: blank, shift 128 bits, latch, set address,
 * then light the row for ROW_ON_CYCLES and blank again.
 * The panel is always blank while data or address are changing,
 * and blank when this function returns.
 */

/* Latch the serial pixel data so it becomes the panel output data. */
static void hub12_pulse_latch(void)
{
    GPIOF->BSRR = 1U << HUB12_LATCH_PIN;
    delay_cycles(20U);
    GPIOF->BSRR = 1U << (HUB12_LATCH_PIN + 16U);
}

/* Multiplex the display: shift pixel bits, latch them, select a row address, and enable output briefly. */
static void matrix_refresh(void)
{
    /* Refresh one multiplexed scan row at a time. */
    for (uint8_t scan_row = 0U; scan_row < MATRIX_SCAN_ROWS; scan_row++)
    {
        OE_OFF();

        /* Send the panel in four 8-column sections. */
        for (uint8_t section = 0U; section < PANEL_SECTIONS; section++)
        {
            for (uint8_t current_plane = 0U;
                 current_plane < DATA_PLANES;
                 current_plane++)
            {
                /* Shift each byte most-significant bit first using DATA and CLOCK. */
                for (int8_t bit = 7; bit >= 0; bit--)
                {
                    uint8_t column = (uint8_t)(section * 8U + (7 - bit));
                    uint8_t logical_row = visual_row_by_plane_and_scan[
                        current_plane][scan_row];
                    uint8_t enabled = framebuffer[(uint32_t)logical_row *
                                                  MATRIX_WIDTH + column] != 0U;

                    GPIOF->BSRR = enabled != 0U
                                ? 1U << (HUB12_DATA_PIN + 16U)
                                : 1U << HUB12_DATA_PIN;
                    GPIOF->BSRR = 1U << HUB12_CLOCK_PIN;
                    delay_cycles(2U);
                    GPIOF->BSRR = 1U << (HUB12_CLOCK_PIN + 16U);
                }
            }
        }

        hub12_pulse_latch();

        /* Select the row address using the panel A/B address pins. */
        uint8_t addr = address_order[scan_row];
        uint32_t a = (addr & 1U) != 0U
                   ? 1U << HUB12_A_PIN
                   : 1U << (HUB12_A_PIN + 16U);
        uint32_t b = (addr & 2U) != 0U
                   ? 1U << HUB12_B_PIN
                   : 1U << (HUB12_B_PIN + 16U);
        GPIOF->BSRR = a | b;
        delay_cycles(20U);

        OE_ON();
        delay_cycles(ROW_ON_CYCLES);
        OE_OFF();
    }
}

/* ---- Game ---- */

/* Generate the next pseudo-random value using a linear-congruential generator. */
static uint32_t random_next(void)
{
    random_state = random_state * 1664525U + 1013904223U;
    return random_state >> 16;   /* high bits: low bits of an LCG are weak */
}

/* True if (x,y) is among the first `count` snake segments. */
/* Check whether a coordinate matches any of the first count snake segments. */
static uint8_t snake_hits(uint8_t x, uint8_t y, uint16_t count)
{
    for (uint16_t i = 0U; i < count; i++)
    {
        if ((snake[i].x == x) && (snake[i].y == y))
        {
            return 1U;
        }
    }
    return 0U;
}

/* Number of cells the snake can ever occupy in the current mode. */
/* Calculate the number of usable cells for the current difficulty mode. */
static uint16_t playfield_cells(void)
{
    return (game_mode == MODE_HARD) ? (uint16_t)((MATRIX_WIDTH - 2U) *
                                                 (MATRIX_HEIGHT - 2U))
                                    : (uint16_t)CELL_COUNT;
}

/* Choose a random free cell for food; hard mode keeps it inside the border. */
static void place_food(void)
{
    /* In hard mode, food must stay inside the border. */
    /* Hard mode reserves the outermost row and column for the wall. */
    uint8_t margin = (game_mode == MODE_HARD) ? 1U : 0U;
    uint8_t span_x = (uint8_t)(MATRIX_WIDTH - 2U * margin);
    uint8_t span_y = (uint8_t)(MATRIX_HEIGHT - 2U * margin);

    do
    {
        food.x = (uint8_t)(margin + random_next() % span_x);
        food.y = (uint8_t)(margin + random_next() % span_y);
    } while (snake_hits(food.x, food.y, snake_length) != 0U);
}

/* Initialize the snake at the center, set its starting direction, and place food. */
static void snake_reset(void)
{
    snake_length = START_LENGTH;
    snake[0] = (cell_t){16U, 8U};
    snake[1] = (cell_t){15U, 8U};
    snake[2] = (cell_t){14U, 8U};
    direction_x = 1;
    direction_y = 0;
    place_food();
}

/* Returns 1 if the game goes on, 0 if the snake died. */
/* Advance one game step; return 0 if a collision/game-ending condition occurs. */
static uint8_t snake_step(void)
{
    /* Calculate the next head coordinate from the current direction. */
    cell_t head = snake[0];
    int16_t next_x = (int16_t)head.x + direction_x;
    int16_t next_y = (int16_t)head.y + direction_y;

    if (game_mode == MODE_HARD)
    {
        /* Border cells are x=0, x=31, y=0, y=15: stepping on one kills. */
        if ((next_x < 1) || (next_x > (int16_t)(MATRIX_WIDTH - 2U)) ||
            (next_y < 1) || (next_y > (int16_t)(MATRIX_HEIGHT - 2U)))
        {
            return 0U;
        }
    }
    else
    {
        if (next_x < 0)
        {
            next_x = MATRIX_WIDTH - 1U;
        }
        else if (next_x >= (int16_t)MATRIX_WIDTH)
        {
            next_x = 0;
        }

        if (next_y < 0)
        {
            next_y = MATRIX_HEIGHT - 1U;
        }
        else if (next_y >= (int16_t)MATRIX_HEIGHT)
        {
            next_y = 0;
        }
    }

    cell_t next = {(uint8_t)next_x, (uint8_t)next_y};

    /* Eating food grows the snake and changes which body cells count as collisions. */
    uint8_t ate_food = (next.x == food.x) && (next.y == food.y);

    /* The tail cell is about to move away, so it is safe to enter
     * unless we are growing this step. */
    uint16_t check_count = (ate_food != 0U) ? snake_length
                                            : (uint16_t)(snake_length - 1U);
    if (snake_hits(next.x, next.y, check_count) != 0U)
    {
        return 0U;
    }

    uint16_t new_length = (ate_food != 0U) ? (uint16_t)(snake_length + 1U)
                                           : snake_length;

    if (new_length > SNAKE_CAPACITY)
    {
        return 0U;
    }

    /* Shift segments from tail toward head so each segment follows the one ahead of it. */
    for (uint16_t i = new_length - 1U; i > 0U; i--)
    {
        snake[i] = snake[i - 1U];
    }
    snake[0] = next;
    snake_length = new_length;

    if (ate_food != 0U)
    {
        /* Board completely full: no free cell for food, so end the game
         * here instead of hanging in place_food(). */
        if (snake_length >= playfield_cells())
        {
            return 0U;
        }
        place_food();
    }
    return 1U;
}

/* Draw a border pixel, optionally skipping alternating pixels in dotted-border mode. */
static void border_pixel(uint8_t x, uint8_t y)
{
#if BORDER_DOTTED
    if (((x + y) & 1U) != 0U)
    {
        return;
    }
#endif
    matrix_set_pixel(x, y, 1U);
}

/* Draw the four edges of the LED matrix. */
static void draw_border(void)
{
    for (uint8_t x = 0U; x < MATRIX_WIDTH; x++)
    {
        border_pixel(x, 0U);
        border_pixel(x, (uint8_t)(MATRIX_HEIGHT - 1U));
    }
    for (uint8_t y = 1U; y < (MATRIX_HEIGHT - 1U); y++)
    {
        border_pixel(0U, y);
        border_pixel((uint8_t)(MATRIX_WIDTH - 1U), y);
    }
}

/* Rebuild the game image from the border, food, and all snake segments. */
static void draw_game(void)
{
    matrix_clear();
    if (game_mode == MODE_HARD)
    {
        draw_border();
    }
    matrix_set_pixel(food.x, food.y, 1U);
    for (uint16_t i = 0U; i < snake_length; i++)
    {
        matrix_set_pixel(snake[i].x, snake[i].y, 1U);
    }
}

/* Convert joystick ADC values into a direction, using a dead zone and blocking direct reversals. */
static void joystick_update_direction(void)
{
    /* Read both axes; only movements beyond the center dead zone change direction. */
    uint16_t x = joystick_read(JOYSTICK_X_CHANNEL);
    uint16_t y = joystick_read(JOYSTICK_Y_CHANNEL);

    if ((x > ADC_CENTER + ADC_DEAD_ZONE) && (direction_x != -1))
    {
        direction_x = 1;
        direction_y = 0;
    }
    else if ((x + ADC_DEAD_ZONE < ADC_CENTER) && (direction_x != 1))
    {
        direction_x = -1;
        direction_y = 0;
    }
    else if ((y > ADC_CENTER + ADC_DEAD_ZONE) && (direction_y != -1))
    {
        direction_x = 0;
        direction_y = 1;
    }
    else if ((y + ADC_DEAD_ZONE < ADC_CENTER) && (direction_y != 1))
    {
        direction_x = 0;
        direction_y = -1;
    }
}

/* ---- Screens ---- */

/* Keep refreshing the multiplexed panel for a given time interval. */
static void refresh_for_ms(uint32_t ms)
{
    /* Save the start time so the wait can finish after the requested duration. */
    uint32_t start = millis();

    while ((uint32_t)(millis() - start) < ms)
    {
        matrix_refresh();
    }
}

/* Wait for a complete button press/release while keeping the display refreshed. */
static void wait_for_button_press(void)
{
    while (joystick_button_pressed() != 0U)  { matrix_refresh(); } /* let go first  */
    while (joystick_button_pressed() == 0U)  { matrix_refresh(); } /* wait for press */
    while (joystick_button_pressed() != 0U)  { matrix_refresh(); } /* wait release   */
    refresh_for_ms(30U);                                           /* debounce       */

    /* Free randomness: press timing differs every run. */
    random_state ^= millis() * 2654435761U;
}

/* Display the 3-2-1 countdown before starting a round. */
static void countdown(void)
{
    for (uint8_t digit = 3U; digit > 0U; digit--)
    {
        draw_digit(digit);
        refresh_for_ms(1000U);
    }
}

/* Draw the welcome screen and wait for the player to press the button. */
static void show_welcome(void)
{
    matrix_clear();
    draw_text(2U, 5U, "WELCOME");
    wait_for_button_press();
}

/* ">" marker pointing at a menu line whose text starts at row y. */
/* Draw the arrow-shaped cursor beside the selected menu option. */
static void draw_marker(uint8_t x, uint8_t y)
{
    matrix_set_pixel(x,                  y,                  1U);
    matrix_set_pixel((uint8_t)(x + 1U),  (uint8_t)(y + 1U),  1U);
    matrix_set_pixel((uint8_t)(x + 2U),  (uint8_t)(y + 2U),  1U);
    matrix_set_pixel((uint8_t)(x + 1U),  (uint8_t)(y + 3U),  1U);
    matrix_set_pixel(x,                  (uint8_t)(y + 4U),  1U);
}

/* Draw the EASY/HARD menu and show which option is selected. */
static void draw_mode_menu(game_mode_t selection)
{
    matrix_clear();
    draw_text(10U, 2U, "EASY");
    draw_text(10U, 9U, "HARD");
    draw_marker(3U, (selection == MODE_EASY) ? 2U : 9U);
}

/* Stick up = EASY, stick down = HARD, button confirms. */
/* Let the player choose a difficulty with up/down and confirm with the button. */
static void select_mode(void)
{
    game_mode_t selection = MODE_EASY;

    while (joystick_button_pressed() != 0U)
    {
        matrix_refresh();                       /* let go of the last press */
    }

    while (joystick_button_pressed() == 0U)
    {
        uint16_t y = joystick_read(JOYSTICK_Y_CHANNEL);

        if (y > ADC_CENTER + ADC_DEAD_ZONE)
        {
            selection = MODE_HARD;
        }
        else if (y + ADC_DEAD_ZONE < ADC_CENTER)
        {
            selection = MODE_EASY;
        }

        draw_mode_menu(selection);
        matrix_refresh();
    }

    while (joystick_button_pressed() != 0U)
    {
        matrix_refresh();
    }
    refresh_for_ms(30U);                        /* debounce */

    random_state ^= millis() * 2654435761U;
    /* Store the difficulty chosen in the menu for the upcoming round. */
    game_mode = selection;
}

/* Display GAME OVER, then the score, and wait for the next button press. */
static void show_game_over(uint16_t score)
{
    matrix_clear();
    draw_text(8U, 2U, "GAME");
    draw_text(8U, 9U, "OVER");
    refresh_for_ms(2000U);      /* cannot be skipped by accident */

    matrix_clear();
    draw_text(6U, 1U, "SCORE");
    draw_number(8U, score);
    wait_for_button_press();    /* press to continue */
}

/* Program entry point: initialize hardware, show startup screens, and run gameplay forever. */
int main(void)
{
    systick_init();
    matrix_gpio_init();
    joystick_init();

    show_welcome();
    select_mode();
    countdown();
    snake_reset();              /* after select_mode: food depends on mode */

    /* Schedule the first snake movement after a short start delay. */
    uint32_t next_move = millis() + 250U;
    while (1)
    {
        draw_game();
        matrix_refresh();

        /* Advance the snake only when its next movement deadline has arrived. */
        if ((int32_t)(millis() - next_move) >= 0)
        {
            joystick_update_direction();
            if (snake_step() == 0U)
            {
                show_game_over((uint16_t)(snake_length - START_LENGTH));
                select_mode();
                countdown();
                snake_reset();
                next_move = millis() + 250U;   /* else the snake fast-forwards */
            }
            else
            {
                next_move += SNAKE_MOVE_MS;
            }
        }
    }
}
