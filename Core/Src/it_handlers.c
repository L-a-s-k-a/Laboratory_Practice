#include <init.h>

void SysTick_Handler(void)
{
    global_tick_count++;
}

void EXTI9_5_IRQHandler(void)
{
    static uint32_t tick_pb8 = 0;
    static uint32_t tick_pb9 = 0;
    static uint32_t tick_pb6 = 0;

    if (READ_BIT(EXTI->PR, EXTI_PR_PR8)) {
        SET_BIT(EXTI->PR, EXTI_PR_PR8);               // Сброс флага
        if (global_tick_count - tick_pb8 >= delay) {  // Антидребезг
            tick_pb8 = global_tick_count;
            btn_mode = 1;
        }
    }
    if (READ_BIT(EXTI->PR, EXTI_PR_PR9)) {
        SET_BIT(EXTI->PR, EXTI_PR_PR9);
        if (global_tick_count - tick_pb9 >= delay) {
            tick_pb9 = global_tick_count;
            btn_freq = 1;
        }
    }
    if (READ_BIT(EXTI->PR, EXTI_PR_PR6)) {
        SET_BIT(EXTI->PR, EXTI_PR_PR6);
        if (global_tick_count - tick_pb6 >= delay) {
            tick_pb6 = global_tick_count;
            btn_select = 1;
        }
    }
}