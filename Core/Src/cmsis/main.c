#include "init.h"

enum { DEBOUNCE_SAMPLES = 20 };

typedef struct {
    bool pressed;
    uint8_t different_samples;
} Button;

/* Global values for Variable Viewer. See README for bit assignments. */
volatile uint32_t lab_mode;
volatile uint32_t lab_buttons;
volatile uint32_t lab_leds;

static void debounce(Button *button, bool sample)
{
    if (sample == button->pressed) {
        button->different_samples = 0;
    } else if (++button->different_samples == DEBOUNCE_SAMPLES) {
        button->pressed = sample;
        button->different_samples = 0;
    }
}

int main(void)
{
    Button input_button = {0};
    Button output_button = {0};
    Button shared_button = {0};
    bool output = false;

    GPIO_Ini();
    Tick_Init();

    for (;;) {
        Tick_Wait();
        debounce(&input_button, GPIO_ReadInputButton());
        debounce(&output_button, GPIO_ReadOutputButton());

        /* Button 1 has priority when both mode buttons are held. */
        bool next_output = !input_button.pressed &&
                           (output_button.pressed || output);
        if (next_output != output) {
            GPIO_SetSharedOutput(next_output);
            shared_button = (Button){0};
            output = next_output;
        }

        if (!output) {
            debounce(&shared_button, GPIO_ReadSharedButton());
        }

        GPIO_SetIndicators(input_button.pressed, output_button.pressed,
                           shared_button.pressed);

        lab_mode = output;
        lab_buttons = (uint32_t)input_button.pressed |
                      ((uint32_t)output_button.pressed << 1) |
                      ((uint32_t)shared_button.pressed << 2);
        lab_leds = lab_buttons | ((uint32_t)output << 3);
    }
}
