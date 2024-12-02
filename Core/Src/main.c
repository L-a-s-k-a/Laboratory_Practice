#include "init.h"
#include "it_handlers.h"

#define GPIO_ODRX(X) 1UL << X

int last_turned_led = -1;  // индекс диода для включения
int controlled_led = 0;    // индекс управляемого диода 
uint32_t led_flick[6] = {
    3333, 3333, 3333, 3333, 3333, 3333
};
unsigned int led_tick_count[6] = {
    0, 0, 0, 0, 0, 0
};
unsigned int led_tick[3][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
};
uint16_t led_period[3][3] = {
    {3333, 1000, 714},
    {2500,  833, 588},
    {2000,  625, 526}
};
uint8_t index_in_range = 0;  // индекс текущего диапазона частот
uint8_t index_of_range = 0;  // индекс частоты в диапазоне
uint8_t flag = 0;

int main(void) {
    GPIO_Ini();
    RCC_Ini();
    ITR_Ini();
    SysTick_Ini();

    while (1) {
        for (int k = 0; k <= last_turned_led; ++k) {
            if (k <= last_turned_led && k >= 0) {
                uint32_t period = led_flick[k];
                if (led_tick_count[k] >= period / 2) {
                    flag = 1;
                    SET_BIT(GPIOC->ODR, 1 << (7 + k));
                    if (led_tick_count[k] >= period) led_tick_count[k] = 0;
                } else {
                    flag = 0;
                    CLEAR_BIT(GPIOC->ODR, 1 << (7 + k));
                }
            }
        }
    }
}