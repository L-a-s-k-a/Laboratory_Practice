#include "init.h"
#include "it_handlers.h"
#include "logic.h"

int main(void) {
    GPIO_Ini();
    RCC_Ini();
    ITR_Ini();
    SysTick_Ini();

    init_periods_and_ticks();
    while (1) {
        manage_leds();
    }
}