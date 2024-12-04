#include "it_handlers.h"
#include "logic.h"

extern led_t led[6];
extern uint32_t led_tick_map[3][3];
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

void SysTick_Handler(void) {
    ++B1_tick;
    ++B2_tick;

    (led_tick_map[0][0])++;
    (led_tick_map[0][1])++;
    (led_tick_map[0][2])++;
    (led_tick_map[1][0])++;
    (led_tick_map[1][1])++;
    (led_tick_map[1][2])++;
    (led_tick_map[2][0])++;
    (led_tick_map[2][1])++;
    (led_tick_map[2][2])++;
}