#include "stm32f4xx.h"
#include "Init.h"
#include "Interrupt.h"

/* ----------------------------------------------------------
 *                ОПРЕДЕЛЕНИЕ РЕЖИМОВ ГИРЛЯНДЫ
 * ---------------------------------------------------------- */
typedef enum {
    MODE_1 = 0,   // режим перелистывания активного светодиода
    MODE_2        // режим последовательного включения + мигания всех
} modes_t;

static modes_t current_mode = MODE_1;  // текущий режим


/* ----------------------------------------------------------
 *                    ТАБЛИЦЫ ПЕРИОДОВ
 * ---------------------------------------------------------- */
static const uint32_t mode1_periods_ms[3] = {1666U, 769U, 416U};  // 3 скорости для режима 1
static const uint32_t led_toggle_ms[3]   = {2500U, 555U, 333U};  // 3 частоты мигания для режима 2

static uint8_t mode1_freq_index = 0;   // индекс скорости режима 1
static uint8_t mode2_freq_index[6] = {0,0,0,0,0,0}; // индексы частот мигания каждого LED в режиме 2


/* ----------------------------------------------------------
 *                        РЕЖИМ 1
 * ---------------------------------------------------------- */
static uint8_t r1_active_led = 0;      // текущий активный светодиод
static uint32_t r1_last_switch_ms = 0; // время последнего переключения


/* ----------------------------------------------------------
 *                        РЕЖИМ 2
 * ---------------------------------------------------------- */
#define R2_DWELL_MS 3000U   // задержка между последовательными включениями LED

static uint8_t r2_num_on = 0;          // сколько светодиодов уже включено
static uint32_t r2_last_dwell_ms = 0;  // время последнего включённого LED

// время последнего мигательного переключения каждого LED
static uint32_t r2_last_toggle_ms[6] = {0,0,0,0,0,0};

// состояние каждого LED в мигании: 1 = включён, 0 = выключен
static uint8_t r2_output_state[6] = {1,1,1,1,1,1};

// выбранный для изменения частоты LED
static uint8_t r2_selected_led = 0;


int main(void)
{
    /* ---------- ИНИЦИАЛИЗАЦИЯ СИСТЕМЫ ---------- */
    Clock_Init_HSE_PLL_168MHz();   // тактирование 168 МГц
    SysTick_Init_1ms();            // системный таймер 1 мс
    Buttons_GPIO_Init();           // кнопки
    Buttons_EXTI_Init();           // внешние прерывания по кнопкам
    LEDs_GPIO_Init();              // GPIO для LED
    ITR_Init();                    // инициализация обработчиков прерываний

    LED_AllOff();                  // гасим все LED

    /* ---------- НАЧАЛЬНАЯ ИНИЦИАЛИЗАЦИЯ РЕЖИМОВ ---------- */
    r1_last_switch_ms = system_time_ms;
    r2_last_dwell_ms = system_time_ms;
    for(uint8_t i=0; i<6; i++)
        r2_last_toggle_ms[i] = system_time_ms;

    r1_active_led = 0;             // первый активный диод режима 1
    LED_SetActive(r1_active_led);  // зажечь активный

    while(1)
    {
        /* === ПЕРЕКЛЮЧЕНИЕ РЕЖИМА ПО КНОПКЕ PA0 ===  */
        if(button1_pressed_flag)
        {
            button1_pressed_flag = 0;          // сброс флага

            // переключаем MODE_1 <-> MODE_2
            current_mode = (current_mode==MODE_1) ? MODE_2 : MODE_1;

            LED_AllOff();  // при смене режима всё гасим


            /* ---------- сброс режима 1 ---------- */
            r1_active_led = 0;
            r1_last_switch_ms = system_time_ms;
            LED_SetActive(r1_active_led);


            /* ---------- сброс режима 2 ---------- */
            LED_AllOff();          // на случай перехода из режима 2

            // сразу включаем первый LED
            LED_On_Index(0);

            r2_num_on = 1;                // уже один включён
            r2_last_dwell_ms = system_time_ms;

            // сброс таблиц состояний светодиодов
            for(uint8_t i=0; i<6; i++){
                r2_last_toggle_ms[i] = system_time_ms;
                r2_output_state[i]   = 1;
                mode2_freq_index[i]  = 0;
            }

            r2_selected_led = 0;  // первый светодиод выбран по умолчанию
        }

        /* === Режим 2 === */
        if(current_mode == MODE_1)
        {
            uint32_t now = system_time_ms;
            uint32_t period = mode1_periods_ms[mode1_freq_index]; // текущий период

            // пришло время переключать активный светодиод?
            if(now - r1_last_switch_ms >= period)
            {
                r1_last_switch_ms = now;

                // следующий диод из 6
                r1_active_led = (r1_active_led+1U) % 6U;

                LED_SetActive(r1_active_led);
            }

            // кнопка меняет скорость
            if(button2_press_event_flag)
            {
                button2_press_event_flag = 0;
                mode1_freq_index = (mode1_freq_index+1U) % 3U;
            }
        }

        /* === РЕЖИМ 2 === */
        else
        {
            uint32_t now = system_time_ms;


            /* Последовательное включение LED */
            if (r2_num_on < 6U)
            {
                // прошло 3 секунды — включаем следующий светодиод
                if (now - r2_last_dwell_ms >= R2_DWELL_MS)
                {
                    LED_On_Index(r2_num_on);
                    r2_num_on++;
                    r2_last_dwell_ms = now;

                    // если включили все 6, инициализируем фазу мигания
                    if (r2_num_on == 6U)
                    {
                        for(uint8_t i=0;i<6;i++){
                            r2_last_toggle_ms[i] = now;
                            r2_output_state[i] = 1;
                            mode2_freq_index[i] = 0;
                        }
                    }
                }
            }

            /* мигание всеми включёнными LED */
            else
            {
                for (uint8_t i = 0; i < 6; i++)
                {
                    uint32_t interval = led_toggle_ms[mode2_freq_index[i]];

                    // Время для переключения состояния?
                    if (now - r2_last_toggle_ms[i] >= interval)
                    {
                        r2_last_toggle_ms[i] = now;

                        if (r2_output_state[i])
                        {
                            LED_Off_Index(i);
                            r2_output_state[i] = 0;
                        }
                        else
                        {
                            LED_On_Index(i);
                            r2_output_state[i] = 1;
                        }
                    }
                }
            }


            /* === ИЗМЕНЕНИЕ ЧАСТОТЫ МЕРЦАНИЯ ПО КНОПКЕ PA5 ===  */ 
            if (button2_press_event_flag)
            {
                button2_press_event_flag = 0;

                // У выбранного LED переключаем частоту 
                mode2_freq_index[r2_selected_led] =
                    (mode2_freq_index[r2_selected_led] + 1U) % 3U;
            }


            /* Выбор активного LED для изменения частоты по кнопке PC13  */
            if (button3_press_event_flag)
            {
                button3_press_event_flag = 0;

                // Циклический выбор LED: 0–5
                r2_selected_led = (r2_selected_led + 1U) % 6U;
            }
        }
    }
}
