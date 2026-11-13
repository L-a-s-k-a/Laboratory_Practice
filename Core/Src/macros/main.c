#include "init.h"

volatile uint32_t lab_mode;
volatile uint32_t lab_buttons;
volatile uint32_t lab_leds;

int main(void)
{
    bool output = false;

    GPIO_Ini();

    while (1) {
        bool s1 = READ_BIT(GPIOE_IDR, INPUT_BUTTON) == 0U;
        bool s2 = READ_BIT(GPIOE_IDR, OUTPUT_BUTTON) == 0U;

        // S1 выбирает вход, S2 — выход.
        if (s1 && output) {
            CLEAR_BIT(GPIOE_MODER, LED_SUPPLY_MODE);
            CLEAR_BIT(GPIOE_MODER, SHARED_MODE);
            output = false;
        } else if (!s1 && s2 && !output) {
            SET_BIT(GPIOE_MODER, SHARED_OUTPUT);
            SET_BIT(GPIOE_MODER, LED_SUPPLY_OUTPUT);
            output = true;
        }

        bool s3 = !output && (READ_BIT(GPIOE_IDR, SHARED_BUTTON) == 0U);
        WRITE_REG(GPIOD_BSRR, s1 ? RED_ON : RED_OFF);
        WRITE_REG(GPIOD_BSRR, s2 ? BLUE_ON : BLUE_OFF);
        WRITE_REG(GPIOD_BSRR, s3 ? ORANGE_ON : ORANGE_OFF);

        lab_mode = output;
        lab_buttons = (uint32_t)s1 | ((uint32_t)s2 << 1) | ((uint32_t)s3 << 2);
        lab_leds = lab_buttons | ((uint32_t)output << 3);
    }
}
