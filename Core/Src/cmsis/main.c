#include "init.h"

volatile uint32_t lab_mode;
volatile uint32_t lab_buttons;
volatile uint32_t lab_leds;

int main(void)
{
    bool output = false;

    GPIO_Ini();

    while (1) {
        bool s1 = GPIO_ReadInputButton();
        bool s2 = GPIO_ReadOutputButton();

        if (s1 && output) {
            GPIO_SetSharedOutput(false);
            output = false;
        } else if (!s1 && s2 && !output) {
            GPIO_SetSharedOutput(true);
            output = true;
        }

        bool s3 = !output && GPIO_ReadSharedButton();
        GPIO_SetIndicators(s1, s2, s3);

        lab_mode = output;
        lab_buttons = (uint32_t)s1 | ((uint32_t)s2 << 1) | ((uint32_t)s3 << 2);
        lab_leds = lab_buttons | ((uint32_t)output << 3);
    }
}
