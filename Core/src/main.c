#include <stdbool.h>
#include "init.h"
#include "it_handlers.h"

#define LED_COUNT 6U
#define SPEED_COUNT 3U // число достпуных частот в каждом массиве
#define NEXT_LED_MS 500U // пауза перед добавлением следующего LED в режиме мерцания
volatile uint32_t plot_led1 = 0U;
volatile uint32_t led_count;    
int main(void)
{
    /* 1-4 встроены в плату, 5-6 подключены снаружи. */
    const uint32_t led_pin[LED_COUNT] = {
        1U << 12U, 1U << 13U, 1U << 14U, 1U << 15U, 1U << 8U, 1U << 9U // связь порядкового номера с битом порта D
    };
    /* 1000 / f: интервал между соседними LED для 0,6 / 1,3 / 2,4 Гц. */
    const uint32_t step_ms[SPEED_COUNT] = {1667U, 769U, 417U};//0.6 Гц, 1.3 Гц, 2.4 Гц. 
    /* 1000 / (2 × f): время одной фазы мерцания для 0,2 / 0,9 / 1,5 Гц. */
    const uint32_t half_period_ms[SPEED_COUNT] = {2500U, 556U, 333U}; // длительность мерцания

    bool blink_mode = false; // f бегущий огонь правда независимое мерцание
    uint32_t current_led = 0U; // индекс диода который горит в бегузем огне
    uint32_t chase_speed_index = 0U; // индекс скорости бегущего огня
    uint32_t last_step; // время последнего шага
    uint32_t active_leds = 0U; // диоды в режиме мерцания
    uint32_t selected_led = 0U; // текущий выбранный диод для изменения частоты во втором режиме
    /* Для каждого LED свой индекс частоты, время смены фазы и состояние. */
    uint32_t blink_speed_index[LED_COUNT] = {0U}; //индексы скорости для каждого диода
    uint32_t last_toggle[LED_COUNT] = {0U}; // отметки времени для каждого диода показывает откуда считать текущий полупериод
    bool led_on[LED_COUNT] = {false}; // состояние диодов

    Board_Init();
    last_step = system_ms;
    led_count = LED_COUNT;

    while (1) {
        uint32_t pressed = Buttons_Pressed();
        uint32_t now = system_ms;
        uint32_t on_pins = 0U;

        /* кнопка 1 меняет режим и запускает последовательность с LED1. */
        if ((pressed & BUTTON_MODE) != 0U) {
            blink_mode = !blink_mode; // меняем режим
            current_led = 0U; // сброс позиции бегущего огня к 0 
            last_step = now; // сброс отсчета интерваа нового режима 

            active_leds = 0U;
        }

        /* кнопка 2 перебирает три частоты новый интервал считаем от нажатия. */
        if ((pressed & BUTTON_SPEED) != 0U) {
            if (blink_mode) {
                blink_speed_index[selected_led] =
                    (blink_speed_index[selected_led] + 1U) % SPEED_COUNT;
                last_toggle[selected_led] = now;
            } else {
                chase_speed_index = (chase_speed_index + 1U) % SPEED_COUNT;
                last_step = now;
            }
        }

        /* кнопка 3 выбирает диод, частоту которого изменит кнопка 2. */
        if (blink_mode && (pressed & BUTTON_SELECT) != 0U) {
            selected_led = (selected_led + 1U) % LED_COUNT;
        }

        if (!blink_mode) {
            /* бегущий огонь: горит один диод, после 6 возвращаемся к 1. */
            uint32_t interval = step_ms[chase_speed_index];
            if (now - last_step >= interval) {
                last_step += interval;
                current_led = (current_led + 1U) % LED_COUNT;
            }
            on_pins = led_pin[current_led];
        } else {
            /* первый диод добавляем сразу, следующие — через каждые 500 мс. */
            uint32_t add_interval = active_leds == 0U ? 0U : NEXT_LED_MS;
            if (active_leds < LED_COUNT && now - last_step >= add_interval) {
                last_step += add_interval;
                led_on[active_leds] = true;
                last_toggle[active_leds] = last_step;
                active_leds++;
            }

            /* каждый активный диод меняет фазу по собственному полупериоду. */
            for (uint32_t i = 0U; i < active_leds; i++) {
                uint32_t interval = half_period_ms[blink_speed_index[i]];
                if (now - last_toggle[i] >= interval) {
                    last_toggle[i] += interval;
                    led_on[i] = !led_on[i];
                }
                if (led_on[i]) {
                    on_pins |= led_pin[i];
                }
            }
        }

        /* младшие биты BSRR включают выбранные LED старшие выключают остальные. */
        uint32_t off_pins = LED_PINS & ~on_pins;
        GPIOD->BSRR = on_pins | (off_pins << 16U);
        GPIOD->BSRR = on_pins | (off_pins << 16U);
        plot_led1 = (on_pins & led_pin[0]) != 0U;
    }
}
