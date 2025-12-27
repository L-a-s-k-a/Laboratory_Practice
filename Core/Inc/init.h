#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F411xE/stm32f411xe.h"
// задаю пины 
#define LED1_PIN  5  // PA5
#define LED2_PIN  5  // PC5
#define LED3_PIN  6  // PA6
#define LED4_PIN  1  // PB1
#define LED5_PIN  6  // PC6
#define LED6_PIN  2  // PB2
//задаю кнопки
#define BTN_USER_PIN 13 // PC13 кнопка переключения режима
#define BTN_FREQ_PIN 12 // PB12 кнопка смены частоты
#define BTN_SEL_PIN   4 // PC4 кнопка выбора светодиода/группы
// частоты мигания для режиме 0 (по паре диодов) тут периоды задаю в мс
#define MODE0_FREQ0_MS 2000                         // Первый период
#define MODE0_FREQ1_MS 588                          // второй период
#define MODE0_FREQ2_MS 454                          // третий период
// частоты мигания для режима 1 (по одному диоду) также в мс
#define MODE1_FREQ0_MS 3333                         //Первый период
#define MODE1_FREQ1_MS 1250                         // второй период
#define MODE1_FREQ2_MS 714                          //третий период

#define STAGING_INTERVAL_MS 500             // задаю задержку перед включением светодиода
#define DEBOUNCE_MS 50                      // антидребезг

// extern переменные (определены в interrupt.c)
extern uint32_t msTicks;            // счётчик времени
extern uint8_t ledMode; // показывает режим работы 0 = парный (mode0), 1 = по одному (mode1)

// mode0 (парный)
extern uint8_t mode0_group_sel;         //выбранная группа светодиодов
extern uint8_t mode0_group_freq[3];     // индексы 0..2
extern uint32_t mode0_pair_tick;        //счётчик времени для текущей пары
extern uint8_t  mode0_pair_index;       // индекст показывающий активную пару светодиодов

// mode1 (по одному)
extern uint8_t mode1_led_sel;           //выюрбранный светодиод
extern uint8_t mode1_led_freq[6];       // индексы 0..2 для каждого LED
extern bool    mode1_led_active[6];     // показывает активность светодиода
extern uint32_t mode1_led_ticks[6];     // счётчики для каждого LED
extern uint32_t staging_tick;           //таймер задержки включения светодиода
extern uint8_t  staging_index;          // индекс включаемого светодиода

// Функции
void RCC_Init(void);        //тактирование
void GPIO_Init(void);       // порты ввода-вывода
void SysTick_Init(void);    // системный таймер
void Interrupt_Init(void);  //внешние прирывания

void Update_Mode0(void);    //обновление логики для парного мигания
void Update_Mode1(void);    //тут для одиночного мигания
