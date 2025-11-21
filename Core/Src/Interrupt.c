#include "Interrupt.h"

extern volatile uint32_t system_time_ms;
extern volatile uint32_t seconds;
extern volatile uint8_t button1_pressed_flag;
extern volatile uint32_t button1_last_irq_time_ms;
extern volatile uint8_t button2_press_event_flag;
extern volatile uint8_t button2_release_event_flag;
extern volatile uint32_t button2_last_irq_time_ms;

volatile uint16_t tectonics = 0;
// volatile uint8_t seconds = 0;
//  Обработчик таймера
void SysTick_Handler(void)
{
    system_time_ms++;
    tectonics++;
    if (tectonics >= 1000)
    {
        seconds += 1;
        tectonics = 0;
    }
}

// Обработчик кнопки PA0
void EXTI0_IRQHandler(void)
{
    /* Проверяем, есть ли запрос по линии 0 (PA0) */
    if (READ_BIT(EXTI->PR, EXTI_PR_PR0) != 0U)
    {
        uint32_t now = system_time_ms;

        /* Антидребезг BTN_DEBOUNCE_MS */
        if ((now - button1_last_irq_time_ms) >= BTN_DEBOUNCE_MS)
        {
            button1_last_irq_time_ms = now;

            /* Сообщаем main(), что кнопку 1 нажали */
            button1_pressed_flag = 1U;
        }

        /* Сбрасываем флаг прерывания по линии 0 */
        SET_BIT(EXTI->PR, EXTI_PR_PR0);
    }
}

// Обработчик кнопки PA5
void EXTI9_5_IRQHandler(void)
{
    /* Проверяем, есть ли запрос по линии 5 (PA5) */
    if (READ_BIT(EXTI->PR, EXTI_PR_PR5) != 0U)
    {
        uint32_t now = system_time_ms;

        /* Антидребезг: не чаще, чем BTN_DEBOUNCE_MS */
        if ((now - button2_last_irq_time_ms) >= BTN_DEBOUNCE_MS)
        {
            button2_last_irq_time_ms = now;

            /* Читаем текущее состояние PA5.
             * Кнопка с подтяжкой к +3.3В:
             *   - в покое: GPIOA->IDR бит 5 = 1
             *   - при нажатии: бит 5 = 0
             */
            if (READ_BIT(GPIOA->IDR, GPIO_IDR_ID5) == 0U)
            {
                /* Вход стал 0 → НАЖАЛИ кнопку 2 (спадающий фронт) */
                button2_press_event_flag = 1U;
            }
            else
            {
                /* Вход стал 1 → ОТПУСТИЛИ кнопку 2 (нарастающий фронт) */
                button2_release_event_flag = 1U;
            }
        }
        /* Сбрасываем флаг прерывания по линии 5 */
        SET_BIT(EXTI->PR, EXTI_PR_PR5);
    }
}