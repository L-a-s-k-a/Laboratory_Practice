#include <init.h>
// Подарящий Д.А. 8Е42 ЛБ2
// Гирлянда из 6 светодиодов (PA5, PA6, PA7, PA9, PA8, PA10)
// Кнопка PB8 - смена режима
// Кнопка PB9 - режим 1: частота 0.6 / 2.2 / 3 Гц
//              режим 2: частота мерцания выбранного светодиода 0.5 / 1.2 / 2 Гц
// Кнопка PB6 - режим 2: включает светодиоды по очереди, после шестого
//              снова выбирает 1-й (он уже включён, просто становится выбранным)

// Для MCUViewer
volatile uint8_t led1_state = 0;   // 1 - горит, 0 - не горит
volatile uint8_t led2_state = 0;
volatile uint8_t led3_state = 0;
volatile uint8_t led4_state = 0;
volatile uint8_t led5_state = 0;
volatile uint8_t led6_state = 0;

volatile uint8_t mode = 0;         // 0 - режим 1, 1 - режим 2

// Режим 1
uint8_t m1_freq = 0;      // 0 - 0.6 Гц, 1 - 2.2 Гц, 2 - 3 Гц
uint8_t m1_step = 0;      // шаг гирлянды
uint32_t m1_last = 0;     // время последнего шага

// Режим 2
uint8_t m2_active = 0;             // сколько светодиодов уже включено
volatile uint8_t m2_select = 0;    // номер выбранного светодиода 1..6 (0 - ещё нет)
uint8_t m2_freq[7];                // частота каждого светодиода по его номеру 1..6 (0 - 0.5, 1 - 1.2, 2 - 2 Гц)


// Включить (state = 1) или выключить (state = 0) светодиод номер num (1..6)
void led_set(uint8_t num, uint8_t state)
{
    if (num == 1) { 
        if (state) 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS5);  
        else 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR5);  
        led1_state = state; 
    }
    if (num == 2) { 
        if (state) 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS6);  
        else 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR6);  
        led2_state = state; 
    }
    if (num == 3) { 
        if (state) 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS7);  
        else 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR7);  
        led3_state = state; 
    }
    if (num == 4) { 
        if (state) 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS9);  
        else 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR9);  
        led4_state = state; 
    }
    if (num == 5) { 
        if (state) 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS8);  
        else 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR8);  
        led5_state = state; 
    }
    if (num == 6) { 
        if (state) 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS10); 
        else 
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR10); 
        led6_state = state; 
    }
}

// Погасить все светодиоды
void all_off(void)
{
    for (uint8_t i = 1; i <= 6; i++)
    {
        led_set(i, 0);
    }
}

// Выполнить шаг гирлянды в режиме 1
void m1_do_step(void)
{
    if (m1_step == 0) { led_set(1, 1); led_set(6, 1); }  // включаем 1 и 6
    if (m1_step == 1) { led_set(2, 1); led_set(5, 1); }  // включаем 2 и 5
    if (m1_step == 2) { led_set(3, 1); led_set(4, 1); }  // включаем 3 и 4
    if (m1_step == 3) { led_set(3, 0); led_set(4, 0); }  // выключаем 3 и 4
    if (m1_step == 4) { led_set(2, 0); led_set(5, 0); }  // выключаем 2 и 5
    if (m1_step == 5) { led_set(1, 0); led_set(6, 0); }  // выключаем 1 и 6
}

int main(void)
{
    __RCC_Config();
    __GPIO_Config();
    __ITR_Config();
    SysTick_Init_Config();

    all_off();
    m1_do_step();                      // старт в режиме 1

    while (1)
    {
        uint32_t now = global_tick_count;

        // Кнопка PB8: смена режима
        if (btn_mode)
        {
            btn_mode = 0;
            all_off();

            if (mode == 0)
            {
                mode = 1;              // в режим 2: всё выключено
                m2_active = 0;
                m2_select = 0;
                for (uint8_t i = 1; i <= 6; i++){
                    m2_freq[i] = 0;
                };
            }
            else
            {
                mode = 0;              // в режим 1: с первого шага
                m1_step = 0;
                m1_last = now;
                m1_do_step();
            }
        }

        // Кнопка PB9: смена частоты
        if (btn_freq)
        {
            btn_freq = 0;

            if (mode == 0)
            {
                m1_freq++;
                if (m1_freq == 3) m1_freq = 0;
            }
            else if (m2_select > 0)       // если уже есть выбранный светодиод
            {
                m2_freq[m2_select]++;
                if (m2_freq[m2_select] == 3) m2_freq[m2_select] = 0;
            }
        }

        // Кнопка PB6: следующий светодиод (режим 2)
        if (btn_select)
        {
            btn_select = 0;

            if (mode == 1)
            {
                m2_select++;
                if (m2_select == 7) m2_select = 1;                  // после шестого снова первый
                if (m2_select > m2_active) m2_active = m2_select;   // новый светодиод - включаем
            }
        }

        // Режим 1
        if (mode == 0)
        {
            uint32_t period = 333;                        // 3 Гц
            if (m1_freq == 0) period = 1667;              // 0.6 Гц
            if (m1_freq == 1) period = 455;               // 2.2 Гц

            if (now - m1_last >= period)
            {
                m1_last = now;
                m1_step++;
                if (m1_step == 6) m1_step = 0;
                m1_do_step();
            }
        }

        // Режим 2
        if (mode == 1)
        {
            for (uint8_t i = 1; i <= 6; i++)
            {
                if (i > m2_active)
                {
                    led_set(i, 0);                        // этот светодиод ещё не включали
                }
                else
                {
                    uint32_t half = 250;                  // 2 Гц
                    if (m2_freq[i] == 0) half = 1000;     // 0.5 Гц
                    if (m2_freq[i] == 1) half = 417;      // 1.2 Гц

                    // первую половину периода горит, вторую не горит
                    if (now % (half * 2) < half) led_set(i, 1);
                    else                         led_set(i, 0);
                }
            }
        }
    }
}