#include "interrupt.h"
#include "init.h" 

uint32_t msTicks = 0;   
uint8_t ledMode = 0; // 0 = парный, 1 = по одному

// mode0 (парные группы)
uint8_t mode0_group_sel = 0;        // выбранная группа для смены частоты
uint8_t mode0_group_freq[3] = {0,0,0};  //массив частот для пары
uint32_t mode0_pair_tick = 0;   
uint8_t  mode0_pair_index = 0;

// mode1 (по одному)
uint8_t mode1_led_sel = 0;
uint8_t mode1_led_freq[6] = {0,0,0,0,0,0};  //частота мегания каждого светодиода
bool    mode1_led_active[6] = {false,false,false,false,false,false};
uint32_t mode1_led_ticks[6] = {0,0,0,0,0,0};
uint32_t staging_tick = 0;
uint8_t  staging_index = 0;

//переменные для антидребезга
uint32_t last_time_user = 0;
uint32_t last_time_freq = 0;
uint32_t last_time_sel  = 0;

uint32_t get_mode0_period_by_index(uint8_t idx)     //преобразую индекс в период (0 режим)
{
    switch(idx) //выбор нужного периода
    {
        case 0: return MODE0_FREQ0_MS;
        case 1: return MODE0_FREQ1_MS;
        case 2: return MODE0_FREQ2_MS;
        default: return MODE0_FREQ0_MS;
    }
}
uint32_t get_mode1_period_by_index(uint8_t idx)// преобразование индекса в период для первого режима
{
    switch(idx)
    {
        case 0: return MODE1_FREQ0_MS;
        case 1: return MODE1_FREQ1_MS;
        case 2: return MODE1_FREQ2_MS;
        default: return MODE1_FREQ0_MS;
    }
}


// обработка системного таймера
void SysTick_Handler(void)
{
    msTicks++;
    mode0_pair_tick++;
    for(uint8_t i=0;i<6;i++) mode1_led_ticks[i]++;
    staging_tick++;
}

// прерывание от кнопки выбора светодиода/группы
void EXTI4_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR4); // сброс флага прерывания

    if(!(GPIOC->IDR & (1U << BTN_SEL_PIN))) //проверка на нажатие
    {
        uint32_t now = msTicks;     //сохраняю текущее время
        if((now - last_time_sel) >= DEBOUNCE_MS)    // проверяю на андидребезг
        {
            if(ledMode == 0) // если активен режим пар
            {
                mode0_group_sel++;
                if(mode0_group_sel >= 3) mode0_group_sel = 0;
            }
            else // если по одному светодиоду
            {
                mode1_led_sel++;
                if(mode1_led_sel >= 6) mode1_led_sel = 0;
            }
            last_time_sel = now;
        }
    }
}

// кнопка переключения режима
void EXTI15_10_IRQHandler(void)
{
    if(READ_BIT(EXTI->PR, EXTI_PR_PR13)) // проверяю прерывание от pc13
    {
        SET_BIT(EXTI->PR, EXTI_PR_PR13);// сбрасываю флаг
        if(!(GPIOC->IDR & (1U << BTN_USER_PIN)))    //проверка по нажатию
        {
            uint32_t now = msTicks;
            if((now - last_time_user) >= DEBOUNCE_MS)   //антидребезг
            {
                ledMode ^= 1;//переключение режима
                // сбрасываю состояние для того чтобы корректно стартануло
                mode0_pair_tick = 0;
                mode0_pair_index = 0;
                staging_tick = 0;
                staging_index = 0;
                for(uint8_t i=0;i<6;i++)    //откючаю светодиоды и срасываю таймеры для них
                {
                    mode1_led_active[i] = false;
                    mode1_led_ticks[i] = 0;
                }
                last_time_user = now;
            }
        }
    }

    if(READ_BIT(EXTI->PR, EXTI_PR_PR12))//проверяю прерывание от кнопки PB12
    {
        SET_BIT(EXTI->PR, EXTI_PR_PR12);// Всё тоже самое
        if(!(GPIOB->IDR & (1U << BTN_FREQ_PIN)))
        {
            uint32_t now = msTicks;
            if((now - last_time_freq) >= DEBOUNCE_MS)
            {
                if(ledMode == 0)
                {
                    // режим 0 смена частоты для выбранной группы
                    uint8_t prev = mode0_group_freq[mode0_group_sel];
                    uint8_t next = prev + 1;
                    if(next >= 3) next = 0;
                    mode0_group_freq[mode0_group_sel] = next;
                    // перезапускаю цикл с начала (первая пара) и с новой частотой
                    mode0_pair_index = 0;  // начинаю с пары 3&4
                    mode0_pair_tick  = 0;  // начинаю новый период с нуля
                }
                else
                {
                    // режим 1 смена частоты для выбранного светодиода
                    uint8_t prev = mode1_led_freq[mode1_led_sel];
                    uint8_t next = prev + 1;
                    if(next >= 3) next = 0;
                    mode1_led_freq[mode1_led_sel] = next;

                    // сбрасываю тик конкретного светодиода чтобы новая частота сразу применялась
                    mode1_led_ticks[mode1_led_sel] = 0;
                }
                last_time_freq = now;
            }
        }
    }
}
