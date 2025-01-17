#include "init.h"

// Глобальные переменные для мониторинга
volatile uint8_t num_leds = 1;        // Количество одновременно включаемых светодиодов
volatile uint8_t button1_state = 0;   // Состояние кнопки D8 (0 = не нажата, 1 = нажата)
volatile uint8_t button2_state = 0;   // Состояние кнопки D9 (0 = не нажата, 1 = нажата)
volatile uint8_t current_led = 0;     // Индекс текущего включенного светодиода
volatile uint8_t leds_on = 0;         // Состояние светодиодов (0 = выкл, 1 = вкл)

int main(void) {
    GPIO_Init(); // Инициализация GPIO

    while (1) {
        // Проверка состояния кнопки D8 (PF12)
        button1_state = (GPIOF_IDR & GPIOF_PIN12) ? 1 : 0;
        if (button1_state) {
            num_leds = (num_leds % 6) + 1;
            while (GPIOF_IDR & GPIOF_PIN12);
        }

        button2_state = (GPIOD_IDR & GPIOD_PIN15) ? 1 : 0;
        if (button2_state) {
            if (num_leds == 6) {
                if (leds_on) {
                    GPIOG_BSRR = (GPIOG_PIN9 | GPIOG_PIN14) << 16;
                    GPIOF_BSRR = (GPIOF_PIN15 | GPIOF_PIN14) << 16;
                    GPIOE_BSRR = (GPIOE_PIN13 | GPIOE_PIN11) << 16;
                    leds_on = 0;
                } else {
                    GPIOG_BSRR = GPIOG_PIN9 | GPIOG_PIN14;
                    GPIOF_BSRR = GPIOF_PIN15 | GPIOF_PIN14;
                    GPIOE_BSRR = GPIOE_PIN13 | GPIOE_PIN11;
                    leds_on = 1;
                }
            } else {
                GPIOG_BSRR = (GPIOG_PIN9 | GPIOG_PIN14) << 16;
                GPIOF_BSRR = (GPIOF_PIN15 | GPIOF_PIN14) << 16;
                GPIOE_BSRR = (GPIOE_PIN13 | GPIOE_PIN11) << 16;

                for (uint8_t i = 0; i < num_leds; i++) {
                    uint8_t led_idx = (current_led + i) % 6;
                    switch (led_idx) {
                        case 0: GPIOG_BSRR = GPIOG_PIN9; break;
                        case 1: GPIOG_BSRR = GPIOG_PIN14; break;
                        case 2: GPIOF_BSRR = GPIOF_PIN15; break;
                        case 3: GPIOE_BSRR = GPIOE_PIN13; break;
                        case 4: GPIOF_BSRR = GPIOF_PIN14; break;
                        case 5: GPIOE_BSRR = GPIOE_PIN11; break;
                    }
                }

                current_led = (current_led + 1) % 6;
            }
            while (GPIOD_IDR & GPIOD_PIN15);
        }
    }
}