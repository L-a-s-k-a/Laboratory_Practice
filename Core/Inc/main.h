#ifndef MAIN_H
#define MAIN_H

#include "init.h"

/* ---- Settings ---- */
#define DEBOUNCE_TICKS      100000UL   /* busy-wait for button debounce */
#define LEDS_COUNT          6U         /* number of LEDs */

/* Variables for STMViewer (defined in main.c) */
extern volatile uint8_t flag;          /* 1 while a button is pressed */
extern volatile uint8_t last_btn;      /* last pressed button (1..3) */
extern volatile uint8_t leds_on;       /* how many LEDs are on (0..6) */
extern volatile uint8_t swapped;       /* 1 - buttons 1 and 2 are swapped */

void delay(volatile uint32_t ticks);

#endif /* MAIN_H */