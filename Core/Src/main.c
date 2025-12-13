#include <stdint.h>
#include "init.h"
#include "Interrupt.h"

// Для начала работы режима 2: по одному светодиоду каждые 0.5 с
volatile uint8_t mode2_ramp_done = 0U;       // 0 - ещё набираем светодиоды, 1 - все включены, можно моргать
volatile uint32_t mode2_last_enable_ms = 0U; // когда последний раз включали новый светодиод

#define MODE2_ENABLE_STEP_MS 500U  // шаг включения новых светодиодов
#define MODE2_ALL_ON_HOLD_MS 1000U // сколько все вместе просто горят до мерцания

// Фазы режима 2:
// 0 - включаем по одному
// 1 - все включены, держим просто включёнными
// 2 - режим мерцания
volatile uint8_t mode2_stage = 0U;
volatile uint8_t mode2_enabled_count = 0U;   // сколько уже включено (0..6)
volatile uint32_t mode2_all_on_time_ms = 0U; // когда все 6 стали гореть
// Время от SysTick, мс и секунды 
volatile uint32_t system_time_ms = 0U;
volatile uint32_t seconds = 0U;

// Флаги событий от кнопок (ставятся в прерываниях) 
volatile uint8_t button1_press_flag = 0U; // Кнопка 1 (смена режима)
volatile uint8_t button2_press_flag = 0U; // Кнопка 2 (смена частоты)
volatile uint8_t button3_press_flag = 0U; // Кнопка 3 (смена номера LED)

volatile uint32_t button1_last_irq_time_ms = 0U; // для антидребезга
volatile uint32_t button2_last_irq_time_ms = 0U;
volatile uint32_t button3_last_irq_time_ms = 0U;

//Кол-во светодиодов
#define LED_COUNT 6U

/*
 * mode = 0 -> Режим 1: пары светодиодов
 * mode = 1 -> Режим 2: индивидуальное мерцание каждого светодиода
 */
volatile uint8_t current_mode = 0U;
 /* --------------------- Режим 1--------------------- */

/*
 * Порядок пар (по индексу светодиода 0..5):
 *  0: 3-й и 4-й светодиоды -> индексы 2 и 3
 *  1: 2-й и 5-й -> индексы 1 и 4
 *  2: 1-й и 6-й -> индексы 0 и 5
 */
volatile uint8_t pair_index = 0U;  // 0..2, какая пара сейчас горит
volatile uint8_t freq1_index = 0U; // 0..2, индекс частоты режима 1

/* Частоты для режима 1: 0.5 Гц, 1.7 Гц, 2.2 Гц
 * Храним период переключения пары (T) в мс
 * 0.5 Гц -> T = 2000 мс
 * 1.7 Гц -> T = 588 мс
 * 2.2 Гц -> T = 455 мс
 */
static const uint32_t mode1_intervals_ms[3] = {2000U, 588U, 455U};
volatile uint32_t last_pair_switch_ms = 0U;

// Режим 2 (индивидуальное мерцание) 

/*
 * Для каждого светодиода храним:
 *  freq2_index[i] — индекс частоты 0..2
 *  last_toggle_ms[i] — момент последнего переключения состояния
 *  led_state[i] — 0 = выключен, 1 = включен
 *
 * Частоты: 0.3 Гц, 0.8 Гц, 1.4 Гц
 * Храним полупериод (T/2) в мс, т.к. мы "переключаем" (toggle) состояние.
 *
 * 0.3 Гц -> T = 3333 мс -> T/2 ≈ 1666 мс
 * 0.8 Гц -> T = 1250 мс -> T/2 = 625 мс
 * 1.4 Гц -> T = 714 мс -> T/2 ≈ 357 мс
 */
volatile uint8_t selected_led = 0U; // какой LED сейчас настраиваем (0..5)
volatile uint8_t freq2_index[LED_COUNT] = {0U, 0U, 0U, 0U, 0U, 0U};
volatile uint32_t last_toggle_ms[LED_COUNT] = {0U};
volatile uint8_t led_state[LED_COUNT] = {0U}; // текущее состояние LED (для режима 2)

static const uint32_t mode2_intervals_ms[3] = {1666U, 625U, 357U};

static void Button1_Task(void);
static void Button2_Task(void);
static void Button3_Task(void);

static void Mode1_Task(void);
static void Mode2_Task(void);

int main(void)
{
    //Тактирование и таймер SysTick
    Clock_Init_HSE_PLL_168MHz();
    SysTick_Init_1ms();

    //Инициализация GPIO
    LEDs_GPIO_Init();
    Buttons_GPIO_Init();
    Buttons_EXTI_Init();

    // Начальное состояние: всё погашено
    LED_AllOff();

    //Обнулим счётчики для режимов

    last_pair_switch_ms = system_time_ms;
    for (uint8_t i = 0; i < LED_COUNT; i++)
    {
        last_toggle_ms[i] = system_time_ms;
        led_state[i] = 0U;
        freq2_index[i] = 0U; // начнём со самой медленной частоты
    }

    selected_led = 0U; // по умолчанию настраиваем первый светодиод

    while (1)
    {
        Button1_Task(); // смена режима
        Button2_Task(); // смена частоты
        Button3_Task(); // смена номера настраиваемого LED (в режиме 2)

        //В зависимости от режима вызываем соответствующую "логику гирлянды"
        if (current_mode == 0U)
        {
            Mode1_Task(); // пары 3-4, 2-5, 1-6
        }
        else
        {
            Mode2_Task(); // индивидуальное мерцание всех 6 светодиодов
        }
    }
}

// Кнопка 1: смена режима работы гирлянды (0 <--> 1) 
static void Button1_Task(void)
{
    if (button1_press_flag == 0U)
        return;

    button1_press_flag = 0U;

    // Переключаем режим: 0 -> 1 -> 0 -> ..
    current_mode ^= 1U; 

    // При смене режима — гасим всё и сбрасываем служебные переменные 
    LED_AllOff();

    if (current_mode == 0U)
    {
        // Переход в режим 1 (пары) 
        pair_index = 0U;
        freq1_index = 0U; // можно начать с самой медленной частоты
        last_pair_switch_ms = system_time_ms;
    }
    else
    {
        // Переход в режим 2 (индивидуальное мерцание) 

        LED_AllOff();

        for (uint8_t i = 0; i < LED_COUNT; i++)
        {
            led_state[i] = 0U;
            last_toggle_ms[i] = system_time_ms;
        }

        selected_led = 0U;

        mode2_stage = 0U;                      // начинаем с набора
        mode2_enabled_count = 0U;              // пока ни один не включен
        mode2_last_enable_ms = system_time_ms; // старт таймера набора
        mode2_all_on_time_ms = 0U;             // пока не все включены
    }
}

/* Кнопка 2:
 *  - в режиме 1: меняет частоту переключения пар (0.5 / 1.7 / 2.2 Гц)
 *  - в режиме 2: меняет частоту мерцания выбранного светодиода
 */
static void Button2_Task(void)
{
    if (button2_press_flag == 0U)
        return;

    button2_press_flag = 0U;

    if (current_mode == 0U)
    {
        // Режим 1 — переключаем частоту для всех пар 
        freq1_index++;
        if (freq1_index >= 3U)
            freq1_index = 0U;
    }
    else
    {
        // Режим 2 — меняем частоту только для выбранного светодиода 
        uint8_t led = selected_led;
        if (led < LED_COUNT)
        {
            freq2_index[led]++;
            if (freq2_index[led] >= 3U)
                freq2_index[led] = 0U;
        }
    }
}

/* Кнопка 3:
 *  - в режиме 2: циклически выбирает светодиод (0..5),
 *    для которого кнопка 2 будет менять частоту
 */
 static void Button3_Task(void)
{
     
    if (button3_press_flag == 0U)
        return;

    button3_press_flag = 0U;

    if (current_mode == 1U)
    {
        selected_led++;
        if (selected_led >= LED_COUNT)
            selected_led = 0U;
    }
}

/* =================== Реализация режимов =================== */

/* Режим 1:
 * Светодиоды включаются и выключаются по 2 штуки:
 *   1) LED3 и LED4 (индексы 2 и 3)
 *   2) LED2 и LED5 (индексы 1 и 4)
 *   3) LED1 и LED6 (индексы 0 и 5)
 * Частота переключения пар задаётся freq1_index.
 */
static void Mode1_Task(void)
{
    if (current_mode != 0U)
        return;

    uint32_t now = system_time_ms;
    uint32_t interval = mode1_intervals_ms[freq1_index];

    if ((now - last_pair_switch_ms) < interval)
        return;

    last_pair_switch_ms = now;

    LED_AllOff();

    switch (pair_index)
    {
    case 0U:
        // 3-й и 4-й светодиоды
        LED_On_Index(2U);
        LED_On_Index(3U);
        break;

    case 1U:
        // 2-й и 5-й светодиоды 
        LED_On_Index(1U);
        LED_On_Index(4U);
        break;

    case 2U:
    default:
        // 1-й и 6-й светодиоды 
        LED_On_Index(0U);
        LED_On_Index(5U);
        break;
    }

    // Переходим к следующей паре 
    pair_index++;
    if (pair_index >= 3U)
        pair_index = 0U;
}

/* Режим 2:
 * Каждый светодиод мигает со своей частотой.
 * Частота каждого задаётся freq2_index[i].
 */
static void Mode2_Task(void)
{
    if (current_mode != 1U)
        return;

    uint32_t now = system_time_ms;

    /* === СТАДИЯ 0: включаем по одному каждые MODE2_ENABLE_STEP_MS === */
    if (mode2_stage == 0U)
    {
        if (mode2_enabled_count < LED_COUNT)
        {
            if ((now - mode2_last_enable_ms) >= MODE2_ENABLE_STEP_MS)
            {
                uint8_t led = mode2_enabled_count; // включаем следующий по порядку (0..5)

                mode2_last_enable_ms = now;

                LED_On__Index(led);
                led_state[led] = 1U;
                last_toggle_ms[led] = now; // от этой точки потом будем считать мерцание

                mode2_enabled_count++;

                // Если только что включили последний, фиксируем момент
                if (mode2_enabled_count >= LED_COUNT)
                {
                    mode2_stage = 1U;           // переходим к стадии "все горят"
                    mode2_all_on_time_ms = now; // запоминаем, когда все загорелись
                }
            }
        }

        // Пока не доросли до полной шестерки — вообще ничего не мигает 
        return;
    }

    /* === СТАДИЯ 1: все 6 горят постоянно ещё MODE2_ALL_ON_HOLD_MS === */
    if (mode2_stage == 1U)
    {

        // Ждём примерно секунду, прежде чем начать мерцание 
        if ((now - mode2_all_on_time_ms) >= MODE2_ALL_ON_HOLD_MS)
        {
            mode2_stage = 2U;

            for (uint8_t i = 0; i < LED_COUNT; i++)
            {
                last_toggle_ms[i] = now;
            }
        }

        return;
    }

    /* === СТАДИЯ 2: обычное мерцание каждого светодиода по своей частоте === */
    if (mode2_stage == 2U)
    {
        for (uint8_t i = 0U; i < LED_COUNT; i++)
        {
            uint8_t fi = freq2_index[i];                // 0..2
            uint32_t interval = mode2_intervals_ms[fi]; // T/2 для данного LED

            if ((now - last_toggle_ms[i]) >= interval)
            {
                last_toggle_ms[i] = now;

                if (led_state[i] == 0U)
                {
                    LED_On_Index(i);
                    led_state[i] = 1U;
                }
                else
                {
                    LED_Off_Index(i);
                    led_state[i] = 0U;
                }
            }
        }
    }
}