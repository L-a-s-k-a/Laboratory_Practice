#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"

extern volatile uint8_t counter1;
extern volatile uint16_t led_periods[7]; // миллисекунды
extern volatile uint8_t led_state[7];
extern volatile uint8_t operation_mode;
extern volatile uint32_t last_blink_time[7];

void task1(void);
void task2(void);