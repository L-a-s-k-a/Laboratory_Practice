#include "init.h"
#include "task.h"
#include "handlers.h"
#include <stdint.h>

volatile uint32_t tick_count = 0;
volatile uint8_t btn9_pressed = 0; // PB9  смена частоты   
volatile uint32_t btn9_press_time = 0;   // время нажатия PB9

volatile uint8_t  btn8_pressed = 0; // PB8 переключение LED
volatile uint32_t btn8_press_time = 0;
volatile uint8_t btn8_hold = 0;

volatile uint8_t counter1 = 0;
volatile uint8_t counter2 = 0;

volatile uint16_t led_periods[7] = {0}; // миллисекунды
volatile uint8_t led_state[7]= {0};
volatile uint8_t operation_mode=0;
volatile uint32_t last_blink_time[7] = {0};

int main(void)
{
    GPIO_Ini();
    RCC_Init();
    IRO_INInt();
    SysTick_Init();

    while (1)
    {
       task1();
       task2();
    }
}