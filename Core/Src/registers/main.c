#include "init.h"

volatile uint32_t lab_mode;
volatile uint32_t lab_buttons;
volatile uint32_t lab_leds;

int main(void)
{
    bool output = false;

    GPIO_Ini();

    while (1) {
        bool s1 = ((*(volatile uint32_t *)0x40021010UL) & (1UL << 7)) == 0U;
        bool s2 = ((*(volatile uint32_t *)0x40021010UL) & (1UL << 8)) == 0U;

        // S1 выбирает вход, S2 — выход.
        if (s1 && output) {
            (*(volatile uint32_t *)0x40021000UL) &= ~(3UL << 20);
            (*(volatile uint32_t *)0x40021000UL) &= ~(3UL << 18);
            output = false;
        } else if (!s1 && s2 && !output) {
            (*(volatile uint32_t *)0x40021000UL) |= (1UL << 18);
            (*(volatile uint32_t *)0x40021000UL) |= (1UL << 20);
            output = true;
        }

        bool s3 = !output && (((*(volatile uint32_t *)0x40021010UL) & (1UL << 9)) == 0U);
        (*(volatile uint32_t *)0x40020C18UL) = s1 ? (1UL << 14) : (1UL << 30);
        (*(volatile uint32_t *)0x40020C18UL) = s2 ? (1UL << 15) : (1UL << 31);
        (*(volatile uint32_t *)0x40020C18UL) = s3 ? (1UL << 13) : (1UL << 29);

        lab_mode = output;
        lab_buttons = (uint32_t)s1 | ((uint32_t)s2 << 1) | ((uint32_t)s3 << 2);
        lab_leds = lab_buttons | ((uint32_t)output << 3);
    }
}
