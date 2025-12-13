#include "Interrupt.h"

//Глобальные переменные
extern volatile uint32_t system_time_ms;
extern volatile uint32_t seconds;

extern volatile uint8_t  button1_press_flag;
extern volatile uint8_t  button2_press_flag;
extern volatile uint8_t  button3_press_flag;

extern volatile uint32_t button1_last_irq_time_ms;
extern volatile uint32_t button2_last_irq_time_ms;
extern volatile uint32_t button3_last_irq_time_ms;

// Локальный счётчик для перевода миллисекунд в секунды 
static volatile uint16_t systick_sub_ms = 0;

//SysTick: вызывается каждый 1 мс (настроен в SysTick_Init_1ms)
void SysTick_Handler(void)
{
    system_time_ms++;

    systick_sub_ms++;
    if (systick_sub_ms >= 1000U)
    {
        systick_sub_ms = 0U;
        seconds++;
    }
}

/* 
   EXTI0: Кнопка 1 (PA0)
   - прерывание по спаду (нажали кнопку)
   - в ISR только ставим флаг + антидребезг
*/
void EXTI0_IRQHandler(void)
{
    // Проверяем, есть ли запрос по линии
    if (READ_BIT(EXTI->PR, EXTI_PR_PR0) != 0U)
    {
        uint32_t now = system_time_ms;

        // Антидребезг – не чаще, чем раз в BTN_DEBOUNCE_MS
        if ((now - button1_last_irq_time_ms) >= BTN_DEBOUNCE_MS)
        {
            button1_last_irq_time_ms = now;

            //Кнопка 1 — считаем событие нажатия 
            button1_press_flag = 1U;
        }

        //Сбрасываем флаг прерывания по линии 0 (записью 1) 
        SET_BIT(EXTI->PR, EXTI_PR_PR0);
    }
}

/*
   EXTI9_5: Кнопка 2 (PA5) и Кнопка 3 (PA6)
   - PA5: настроена на оба фронта, но реагируем только когда уровень = 0 (нажатие)
   - PA6: настроена только на спад (нажатие)
*/
void EXTI9_5_IRQHandler(void)
{
    uint32_t now = system_time_ms;

    //Кнопка 2: PA5 / EXTI5 
    if (READ_BIT(EXTI->PR, EXTI_PR_PR5) != 0U)
    {
        if ((now - button2_last_irq_time_ms) >= BTN_DEBOUNCE_MS)
        {
            button2_last_irq_time_ms = now;

            /* Смотрим текущее состояние пина PA5:
               - pull-up, в покое = 1
               - при нажатии = 0
               Реагируем только на "нажатие" (уровень 0). */
            if ((GPIOA->IDR & GPIO_IDR_ID5) == 0U)
            {
                button2_press_flag = 1U;
            }
        }

        //Сбрасываем флаг прерывания по линии 5 
        SET_BIT(EXTI->PR, EXTI_PR_PR5);
    }

    //Кнопка 3: PA6 / EXTI6
    if (READ_BIT(EXTI->PR, EXTI_PR_PR6) != 0U)
    {
        if ((now - button3_last_irq_time_ms) >= BTN_DEBOUNCE_MS)
        {
            button3_last_irq_time_ms = now;
            /* Кнопка 3 реагирует только на "нажатие" (у нас триггер по спаду),
               поэтому просто ставим флаг события. */
            button3_press_flag = 1U;
        }

        //Сбрасываем флаг прерывания по линии 6
        SET_BIT(EXTI->PR, EXTI_PR_PR6);
    }
}