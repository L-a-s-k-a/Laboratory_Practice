#include "logic.h"

uint32_t led_tick_map[3][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
};
uint16_t led_period_map[3][3] = {
    {3333, 1000, 714},
    {2500,  833, 588},
    {2000,  625, 526}
};
int8_t last_turned_led = -1; // индекс диода для включения
int8_t controlled_led = 0;  // индекс управляемого диода
led_t led[6];
uint32_t B1_tick = 0, B2_tick = 0;
uint8_t B1_pressed = 0, B2_pressed = 0;
uint8_t B1_long_press_registered = 0;
uint8_t B2_long_press_registered = 0;

uint8_t led_1_state = 0;    // для просмотра состояния в MCUViewer
uint8_t led_2_state = 2;    // для просмотра состояния в MCUViewer
uint8_t led_3_state = 4;    // для просмотра состояния в MCUViewer
uint8_t led_4_state = 6;    // для просмотра состояния в MCUViewer
uint8_t led_5_state = 8;    // для просмотра состояния в MCUViewer
uint8_t led_6_state = 10;   // для просмотра состояния в MCUViewer
uint8_t *led_state[6];      // для просмотра состояния в MCUViewer

void B1_interrupt() {
    B1_pressed = READ_BIT(GPIOC->IDR, GPIO_IDR_ID0) != 0;
    if (!B1_pressed && B1_tick >= BTN_DELAY_FILTER) {
        // кнопку отпустили
        if (!B1_long_press_registered) B1_short_press();
    } else if (B1_pressed) {
        B1_long_press_registered = 0;
        B1_tick = 0;
    }
}

void B2_interrupt() {
    B2_pressed = READ_BIT(GPIOC->IDR, GPIO_IDR_ID3) != 0;
    if (!B2_pressed && B2_tick >= BTN_DELAY_FILTER) {
        // кнопку отпустили
        if (!B2_long_press_registered) B2_short_press();
    } else if (B2_pressed) {
        B2_long_press_registered = 0;
        B2_tick = 0;
    }
}

void B1_short_press() {
    int8_t k = controlled_led;
    if (++(led[k].index_in_range) == 3) {
        led[k].index_in_range = 0;
    }
    led[k].period = &(led_period_map[led[k].index_of_range][led[k].index_in_range]);
    led[k].tick = &(led_tick_map[led[k].index_of_range][led[k].index_in_range]);
}
void B2_short_press() {
    int8_t k = controlled_led;
    if (++(led[k].index_of_range) == 3) {
        led[k].index_of_range = 0;
    }
    led[k].period = &(led_period_map[led[k].index_of_range][led[k].index_in_range]);
    led[k].tick = &(led_tick_map[led[k].index_of_range][led[k].index_in_range]);
}

void B1_long_press() {
    if (++last_turned_led == 6) {
        last_turned_led = -1;
        CLEAR_BIT(GPIOC->ODR, 0b111111 << 7);
    } else if (last_turned_led >= 0) {
        SET_BIT(GPIOC->ODR, 1 << (7 + last_turned_led));
    }
}
void B2_long_press() {
    if (++controlled_led == 6) controlled_led = 0;
}

void init_periods_and_ticks() {
    for (int i = 0; i < 6; ++i) {
        led[i].tick = &(led_tick_map[0][0]);
        led[i].period = &(led_period_map[0][0]);
        led[i].index_in_range = 0;
        led[i].index_of_range = 0;
    }
    led_state[0] = &led_1_state;
    led_state[1] = &led_2_state;
    led_state[2] = &led_3_state;
    led_state[3] = &led_4_state;
    led_state[4] = &led_5_state;
    led_state[5] = &led_6_state;
}

void manage_leds() {
    for (int k = 0; k <= last_turned_led; ++k) {
        if (k <= last_turned_led && k >= 0) {
            uint32_t period = *led[k].period;
            if (*(led[k].tick) >= period / 2) {
                *led_state[k] = k*2 + 1; // для наблюдения состояния

                SET_BIT(GPIOC->ODR, 1 << (7 + k));
                if (*(led[k].tick) >= period) *(led[k].tick) = 0;
            } else {
                *led_state[k] = k*2;     // для наблюдения состояния

                CLEAR_BIT(GPIOC->ODR, 1 << (7 + k));
            }
        }
    }
    if (B2_pressed && B2_tick >= LONG_PRESS_DURATION) {
        B2_long_press();
        B2_long_press_registered = 1;
        B2_tick = 0;
    }
    if (B1_pressed && B1_tick >= LONG_PRESS_DURATION) {
        B1_long_press();
        B1_long_press_registered = 1;
        B1_tick = 0;
    }
}
