#include "init.h"
#include "it_handlers.h"

#define BUTTON_COUNT 3U
#define DEBOUNCE_MS 25U

volatile uint32_t system_ms; // счетчик мс с момента запуска systick

/* Для каждой кнопки EXTI запоминает время последнего фронта. */
static volatile uint32_t last_button_edge[BUTTON_COUNT];

/* SysTick вызывает обработчик раз в 1 мс. */
void SysTick_Handler(void) // прерываение таймера systick при прерывании ядро само переходит в функцию
{
    system_ms++;
}

/* PA0: запись 1 в PR0 очищает флаг линии. */
void EXTI0_IRQHandler(void) // 
{
    EXTI->PR = EXTI_PR_PR0; // очистка флага события чтобы блок понял что событие принято
    last_button_edge[0] = system_ms;
}

/* PA1: кнопка смены частоты. */
void EXTI1_IRQHandler(void)
{
    EXTI->PR = EXTI_PR_PR1;
    last_button_edge[1] = system_ms;
}

/* PA2: кнопка выбора следующего LED. */
void EXTI2_IRQHandler(void)
{
    EXTI->PR = EXTI_PR_PR2;
    last_button_edge[2] = system_ms;
}

uint32_t Buttons_Pressed(void)
{
    static uint32_t stable_buttons;
    uint32_t pressed = 0U;

    for (uint32_t i = 0U; i < BUTTON_COUNT; i++) {
        uint32_t pin = 1U << i;
        uint32_t level = GPIOA->IDR & pin;
        uint32_t edge_time = last_button_edge[i];
        uint32_t now = system_ms;

        /* Каждый новый фронт заново запускает ожидание 25 мс. */
        if (now - edge_time < DEBOUNCE_MS || level == (stable_buttons & pin)) {
            continue;
        }

        if (level != 0U) {
            stable_buttons |= pin;
            pressed |= pin;
        } else {
            /* Устойчивое отпускание позволяет принять следующее нажатие. */
            stable_buttons &= ~pin;
        }
    }

    return pressed;
}
