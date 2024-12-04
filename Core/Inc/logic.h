#ifndef LOGIC_H
#define LOGIC_H

#include <stdint.h>
#include "stm32f429xx.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

#define BTN_DELAY_FILTER 100
#define LONG_PRESS_DURATION 2000

typedef struct {
    uint32_t *tick;
    uint16_t *period;
    uint8_t index_of_range;
    uint8_t index_in_range;
} led_t;

void B1_interrupt();
void B2_interrupt();
void B1_short_press();
void B2_short_press();
void B1_long_press();
void B2_long_press();
void init_periods_and_ticks();
void manage_leds();

#endif