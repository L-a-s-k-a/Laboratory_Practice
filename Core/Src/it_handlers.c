#include "it_handlers.h"
#include "logic.h"

// extern unsigned int *led_tick[6];
extern led_info led[6];
extern uint32_t B1_tick, B2_tick;
extern uint8_t B1_pressed, B2_pressed;

void EXTI0_IRQHandler(void) {
    SET_BIT(EXTI->PR, EXTI_PR_PR0);
    B1_interrupt();
}

void EXTI3_IRQHandler(void) {
    SET_BIT(EXTI->PR, EXTI_PR_PR3);
    B2_interrupt();
}

// одновременное мигание для диодов с одинаковой частотой.
// обработка дребезга, доработать.
// вынести логику в отдельный файл.

void SysTick_Handler(void) {
    ++B1_tick;
    ++B2_tick;
    
    (*(led[0].tick))++;
    (*(led[1].tick))++;
    (*(led[2].tick))++;
    (*(led[3].tick))++;
    (*(led[4].tick))++;
    (*(led[5].tick))++;
}