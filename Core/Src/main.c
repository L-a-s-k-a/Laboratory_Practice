#include <stdint.h>
#include "stm32f4xx.h"

/* =========================================================
 * Variant 6
 *
 * Button 1 - PC13 (on-board USER button):
 *   mode = 0 -> turn LEDs ON one by one
 *   mode = 1 -> turn LEDs OFF one by one
 *
 * Button 2 - PA3 (external HW-483):
 *   released = HIGH
 *   pressed  = LOW
 *   switches mode between ON and OFF
 *
 * LEDs:
 *   Green - PB0
 *   Blue  - PB7
 *   Red   - PB14
 * =========================================================
 */

volatile uint8_t mode = 0;       // 0 = ON mode, 1 = OFF mode
volatile uint8_t led_count = 0;  // Number of active LEDs: 0...3


/* =========================================================
 * PB7 - USER-DEFINED DIRECTIVES AND MACROS
 * =========================================================
 */

/* GPIOB register addresses */
#define USER_GPIOB_MODER      (*(volatile uint32_t *)(0x40020400UL + 0x00UL))
#define USER_GPIOB_OTYPER     (*(volatile uint32_t *)(0x40020400UL + 0x04UL))
#define USER_GPIOB_OSPEEDR    (*(volatile uint32_t *)(0x40020400UL + 0x08UL))
#define USER_GPIOB_PUPDR      (*(volatile uint32_t *)(0x40020400UL + 0x0CUL))
#define USER_GPIOB_BSRR       (*(volatile uint32_t *)(0x40020400UL + 0x18UL))

/* PB7 masks */
#define PB7_MODER_MASK        (3UL << 14)//3`11 Analog
#define PB7_MODER_OUTPUT      (1UL << 14)

#define PB7_OTYPER_MASK       (1UL << 7)

#define PB7_OSPEED_MASK       (3UL << 14)
#define PB7_OSPEED_MEDIUM     (1UL << 14)

#define PB7_PUPDR_MASK        (3UL << 14)

#define PB7_SET_MASK          (1UL << 7)
#define PB7_RESET_MASK        (1UL << 23)

/* User macros */ 
#define USER_SET_BITS(REG, MASK)      ((REG) |= (MASK))//own tools
#define USER_CLEAR_BITS(REG, MASK)    ((REG) &= ~(MASK))
#define USER_WRITE_REG(REG, VALUE)    ((REG) = (VALUE))


/* =========================================================
 * Software debounce
 * =========================================================
 */

void delay_debounce(void)//
{
    volatile uint32_t i;

    for (i = 0; i < 150000; i++)
    {
    }
}


/* =========================================================
 * Update LED states according to led_count
 * =========================================================
 */

void update_leds(void)
{
    /* =====================================================
     * GREEN LED PB0
     * Direct memory access
     * ===================================================== */

    if (led_count >= 1)
    {
        /* PB0 SET */
        *(volatile uint32_t *)(0x40020400UL + 0x18UL) =
            (1UL << 0);
    }
    else
    {
        /* PB0 RESET */
        *(volatile uint32_t *)(0x40020400UL + 0x18UL) =
            (1UL << 16);
    }


    /* =====================================================
     * BLUE LED PB7
     * User-defined macro
     * ===================================================== */

    if (led_count >= 2)
    {
        USER_WRITE_REG(USER_GPIOB_BSRR, PB7_SET_MASK);
    }
    else
    {
        USER_WRITE_REG(USER_GPIOB_BSRR, PB7_RESET_MASK);
    }


    /* =====================================================
     * RED LED PB14
     * CMSIS
     * ===================================================== */

    if (led_count >= 3)
    {
        GPIOB->BSRR = (1UL << 14);
    }
    else
    {
        GPIOB->BSRR = (1UL << 30);
    }
}


/* =========================================================
 * GPIO initialization
 * =========================================================
 */

void GPIO_Init(void)
{
    /* =====================================================
     * Enable GPIOA, GPIOB and GPIOC clocks
     * ===================================================== */

    SET_BIT(
        RCC->AHB1ENR,
        RCC_AHB1ENR_GPIOAEN |
        RCC_AHB1ENR_GPIOBEN |
        RCC_AHB1ENR_GPIOCEN
    );


    /* =====================================================
     * PB0 GREEN LED
     * DIRECT MEMORY ACCESS
     * ===================================================== */

    /* PB0 -> Output mode */
    *(volatile uint32_t *)(0x40020400UL + 0x00UL) &=
        ~(3UL << 0);

    *(volatile uint32_t *)(0x40020400UL + 0x00UL) |=
        (1UL << 0);


    /* PB0 -> Push-Pull */
    *(volatile uint32_t *)(0x40020400UL + 0x04UL) &=
        ~(1UL << 0);


    /* PB0 -> Medium speed */
    *(volatile uint32_t *)(0x40020400UL + 0x08UL) &=
        ~(3UL << 0);

    *(volatile uint32_t *)(0x40020400UL + 0x08UL) |=
        (1UL << 0);


    /* PB0 -> No Pull-Up / Pull-Down */
    *(volatile uint32_t *)(0x40020400UL + 0x0CUL) &=
        ~(3UL << 0);


    /* =====================================================
     * PB7 BLUE LED
     * USER-DEFINED DIRECTIVES / MACROS
     * ===================================================== */

    /* PB7 -> Output mode */
    USER_CLEAR_BITS(
        USER_GPIOB_MODER,
        PB7_MODER_MASK
    );

    USER_SET_BITS(
        USER_GPIOB_MODER,
        PB7_MODER_OUTPUT
    );


    /* PB7 -> Push-Pull */
    USER_CLEAR_BITS(
        USER_GPIOB_OTYPER,
        PB7_OTYPER_MASK
    );


    /* PB7 -> Medium speed */
    USER_CLEAR_BITS(
        USER_GPIOB_OSPEEDR,
        PB7_OSPEED_MASK
    );

    USER_SET_BITS(
        USER_GPIOB_OSPEEDR,
        PB7_OSPEED_MEDIUM
    );


    /* PB7 -> No Pull-Up / Pull-Down */
    USER_CLEAR_BITS(
        USER_GPIOB_PUPDR,
        PB7_PUPDR_MASK
    );


    /* =====================================================
     * PB14 RED LED
     * CMSIS
     * ===================================================== */

    /* PB14 -> Output mode */
    MODIFY_REG(
        GPIOB->MODER,
        (3UL << (14 * 2)),
        (1UL << (14 * 2))
    );


    /* PB14 -> Push-Pull */
    CLEAR_BIT(
        GPIOB->OTYPER,
        (1UL << 14)
    );


    /* PB14 -> Medium speed */
    MODIFY_REG(
        GPIOB->OSPEEDR,
        (3UL << (14 * 2)),
        (1UL << (14 * 2))
    );


    /* PB14 -> No Pull-Up / Pull-Down */
    CLEAR_BIT(
        GPIOB->PUPDR,
        (3UL << (14 * 2))
    );


    /* =====================================================
     * PC13 - BUTTON 1
     * On-board USER button
     * ===================================================== */

    /* PC13 -> Input */
    CLEAR_BIT(
        GPIOC->MODER,
        (3UL << (13 * 2))
    );


    /* PC13 -> No Pull-Up / Pull-Down */
    CLEAR_BIT(
        GPIOC->PUPDR,
        (3UL << (13 * 2))
    );


    /* =====================================================
     * PA3 - BUTTON 2
     * External HW-483
     *
     * Measured behavior:
     * released = HIGH
     * pressed  = LOW
     *
     * Module provides its own logic level,
     * therefore internal pull resistors are disabled.
     * ===================================================== */

    /* PA3 -> Input */
    CLEAR_BIT(
        GPIOA->MODER,
        (3UL << (3 * 2))
    );


    /* PA3 -> No Pull-Up / Pull-Down */
    CLEAR_BIT(
        GPIOA->PUPDR,
        (3UL << (3 * 2))
    );


    /* Initial state */
    mode = 0;
    led_count = 0;

    update_leds();
}


/* =========================================================
 * Main
 * =========================================================
 */

int main(void)
{
    GPIO_Init();

    while (1)
    {
        /* =================================================
         * BUTTON 2 - PA3
         * External HW-483
         *
         * Released = HIGH
         * Pressed  = LOW
         *
         * Every press changes:
         * mode 0 <-> mode 1
         * =================================================
         */

        if ((GPIOA->IDR & (1UL << 3)) == 0)
        {
            delay_debounce();

            /* Check again after debounce */
            if ((GPIOA->IDR & (1UL << 3)) == 0)
            {
                /* Toggle ON/OFF mode */
                mode ^= 1U;


                /* Wait until external button is released
                 *
                 * Pressed  = LOW
                 * Released = HIGH
                 */
                while ((GPIOA->IDR & (1UL << 3)) == 0)
                {
                }


                delay_debounce();
            }
        }


        /* =================================================
         * BUTTON 1 - PC13
         * On-board USER button
         * =================================================
         */

        if ((GPIOC->IDR & (1UL << 13)) != 0)
        {
            delay_debounce();

            /* Check again after debounce */
            if ((GPIOC->IDR & (1UL << 13)) != 0)
            {
                /* =========================================
                 * ON MODE
                 * ========================================= */

                if (mode == 0)
                {
                    if (led_count < 3)
                    {
                        led_count++;
                    }
                }


                /* =========================================
                 * OFF MODE
                 * ========================================= */

                else
                {
                    if (led_count > 0)
                    {
                        led_count--;
                    }
                }


                /* Apply new state to LEDs */
                update_leds();


                /* Wait until PC13 button is released */
                while ((GPIOC->IDR & (1UL << 13)) != 0)
                {
                }


                delay_debounce();
            }
        }
    }
}
