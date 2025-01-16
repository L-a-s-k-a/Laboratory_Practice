#include "init.h"

int main(void) {
    GPIO_Init();

    uint8_t num_leds = 1;
    uint8_t leds_on = 0;

    while (1) {

        if (GPIOF_IDR & GPIOF_PIN12) {
            num_leds = (num_leds % 6) + 1;
            while (GPIOF_IDR & GPIOF_PIN12);
        }

        if (GPIOD_IDR & GPIOD_PIN15) {
            if (leds_on) {
                GPIOG_BSRR = (GPIOG_PIN9 | GPIOG_PIN14) << 16;
                GPIOF_BSRR = (GPIOF_PIN15 | GPIOF_PIN14) << 16;
                GPIOE_BSRR = (GPIOE_PIN13 | GPIOE_PIN11) << 16;
                leds_on = 0;
            } else {
                if (num_leds >= 1) GPIOG_BSRR = GPIOG_PIN9;  // D0
                if (num_leds >= 2) GPIOG_BSRR = GPIOG_PIN14; // D1
                if (num_leds >= 3) GPIOF_BSRR = GPIOF_PIN15; // D2
                if (num_leds >= 4) GPIOE_BSRR = GPIOE_PIN13; // D3
                if (num_leds >= 5) GPIOF_BSRR = GPIOF_PIN14; // D4
                if (num_leds >= 6) GPIOE_BSRR = GPIOE_PIN11; // D5
                leds_on = 1;
            }
            while (GPIOD_IDR & GPIOD_PIN15);
        }
    }
}
