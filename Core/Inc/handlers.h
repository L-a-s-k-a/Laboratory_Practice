#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"

extern volatile uint32_t tick_count;
extern volatile uint8_t btn9_pressed; // PB9  смена частоты 
extern volatile uint32_t btn9_press_time;   // время нажатия PB9

extern volatile uint8_t  btn8_pressed; // PB8 переключение LED
extern volatile uint32_t btn8_press_time;
extern volatile uint8_t pwm_mode;

void EXTI9_5_IRQHandler(void);
void TIM1_UP_TIM10_IRQHandler(void);