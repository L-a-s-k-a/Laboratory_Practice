#include <stdint.h>
#include "init.h"
#include "Interrupt.h"
// Посылаю всем привет из второй лабораторной!
extern uint32_t SystemCoreClock;
volatile uint32_t g_msTicks = 0U; // глобальный счётчик миллисекунд

#define LED_COUNT 6U

/* Режимы работы светодиодов */
#define LED_MODE_BLINK 0U
#define LED_MODE_ON 1U

/* Частоты мигания: 0.2 Гц, 0.8 Гц, 1.3 Гц
 * Храним "половину периода" в мс (интервал между переключениями состояния)
 * 0.2 Гц → T = 5000 мс, T/2 = 2500
 * 0.8 Гц → T = 1250 мс, T/2 = 625
 * 1.3 Гц → T ≈ 770 мс, T/2 ≈ 385
 */
#define BLINK_MODE_COUNT 3U
static const uint32_t g_blinkIntervals[BLINK_MODE_COUNT] = {
    2500U, 625U, 385U};

// Общее время от SysTick (мс)
volatile uint32_t system_time_ms = 0U;

/* --- Кнопка 1 (PA0) --- */
volatile uint8_t button1_pressed_flag = 0U;
volatile uint32_t button1_last_irq_time_ms = 0U;

/* --- Кнопка 2 (PA5) --- */
volatile uint8_t button2_press_event_flag = 0U;   // фронт нажатия (PA5=0)
volatile uint8_t button2_release_event_flag = 0U; // фронт отпускания (PA5=1)
volatile uint32_t button2_last_irq_time_ms = 0U;

volatile uint8_t button2_is_pressed = 0U;      // сейчас реально зажата (после дебаунса)
volatile uint32_t button2_press_start_ms = 0U; // когда нажали (для измерения длительности)

/* --- Светодиоды --- */
volatile uint8_t led_mode = LED_MODE_BLINK;
volatile uint8_t blink_mode_index = 0U; // 0..2
volatile uint32_t last_blink_time_ms = 0U;
volatile uint8_t blink_led_is_on = 0U; // 0 = выкл, 1 = горит

#define LED_INDEX_NONE 255U
volatile uint8_t current_led_index = LED_INDEX_NONE; // 0..5

extern volatile uint8_t seconds = 0;

void Button1_Task(void)
{
    if (button1_pressed_flag == 0U)
        return;

    button1_pressed_flag = 0U;

    if (current_led_index == LED_INDEX_NONE)
    {
        current_led_index = 0U;
    }
    else
    {
        current_led_index++;
        if (current_led_index >= LED_COUNT)
            current_led_index = 0U;
    }

    if (led_mode == LED_MODE_ON)
    {
        LED_AllOff();
        LED_On_Index(current_led_index);
    }
}

void Button2_Task(void)
{
    uint32_t now = system_time_ms;

    // Нажали
    if (button2_press_event_flag != 0U)
    {
        button2_press_event_flag = 0U;

        button2_is_pressed = 1U;
        button2_press_start_ms = now;
    }

    // Отпустили
    if (button2_release_event_flag != 0U)
    {
        button2_release_event_flag = 0U;

        if (button2_is_pressed != 0U)
        {
            button2_is_pressed = 0U;

            uint32_t press_time = now - button2_press_start_ms;

            if (press_time >= BTN2_LONG_MS)
            {
                // Долгое нажатие — смена режима
                if (led_mode == LED_MODE_BLINK)
                {
                    led_mode = LED_MODE_ON;

                    // В режиме "светится":
                    LED_AllOff();

                    if (current_led_index != LED_INDEX_NONE)
                    {
                        // Если активный LED уже выбран — зажечь только его
                        LED_On_Index(current_led_index);
                        blink_led_is_on = 1U;
                    }
                    else
                    {
                        // Активный ещё не выбран — просто ничего не светим
                        blink_led_is_on = 0U;
                    }
                }
                else
                {
                    // Переход в режим мигания
                    led_mode = LED_MODE_BLINK;

                    LED_AllOff();
                    blink_led_is_on = 0U;
                    last_blink_time_ms = now;
                }
            }
            else
            {
                // Короткое нажатие — смена частоты мигания
                blink_mode_index++;
                if (blink_mode_index >= BLINK_MODE_COUNT)
                {
                    blink_mode_index = 0U;
                }
            }
        }
    }
}

void Blink_Task(void)
{
    // Если режим не "мигание" ничего не делаем
    if (led_mode != LED_MODE_BLINK)
    {
        return;
    }

    // Если ещё не выбрали активный светодиод тоже ничего не делаем
    if (current_led_index == LED_INDEX_NONE)
    {
        return;
    }

    uint32_t now = system_time_ms;
    uint32_t interval = g_blinkIntervals[blink_mode_index];

    if ((now - last_blink_time_ms) >= interval)
    {
        last_blink_time_ms = now;

        if (blink_led_is_on == 0U)
        {
            LED_AllOff();
            LED_On_Index(current_led_index);
            blink_led_is_on = 1U;
        }
        else
        {
            LED_AllOff();
            blink_led_is_on = 0U;
        }
    }
}



int main(void)
{
    Clock_Init_HSE_PLL_168MHz();
    SysTick_Init_1ms();

    LEDs_GPIO_Init();
    Buttons_GPIO_Init();
    Buttons_EXTI_Init();

    LED_AllOff();

    SET_BIT(EXTI->PR, (1U << 0) | (1U << 5));
    MCO_init();
    while (1)
    {
        Button1_Task(); // переключение активного светодиода
        Button2_Task(); // смена частоты / режима
        Blink_Task();   // мигаем в нужном режиме
    }
}

/*



*/