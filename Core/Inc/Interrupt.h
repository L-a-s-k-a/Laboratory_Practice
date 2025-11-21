#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "init.h"

/*
----------------------------
Перменные для КНОПОК
-----------------------------
*/
// длительное удержание и антидребезг

// вспомогательные переменные
static volatile uint32_t g_btn1_last_time = 0;
static volatile uint32_t g_btn2_last_time = 0;
static volatile uint8_t g_btn2_pressed = 0;
static volatile uint32_t g_btn2_press_ms = 0;

// полупериоды (мс) для трёх частот: 0.2 Гц, 0.8 Гц, 1.3 Гц
static const uint32_t g_half_period_ms[3] = {
    2500U, // 0.2 Гц
    625U,  // 0.8 Гц
    385U   // 1.3 Гц
};
// 1000 -> 1 с. Полный период == 1000/0.2 = 5000мс. Полупериод = 2500 мс
// Полный период = 1000 / 0.8 = 1250 мс. Полупериод = 625 мс
// Аналогично 385 мс.
/*
---------------------------
Переменные для СВЕТОДОИДОВ
---------------------------
*/
// Состояния светодиодов
extern volatile uint8_t g_led_mode; // 0 = мигание, 1 = постоянный свет
// Активный светодиод (0..5)
extern volatile uint8_t g_active_led;
// Индекс частоты: 0 → медленная, 1 → средняя, 2 → быстрая [0.2 Гц, 0.8 Гц, 1.3 Гц]
extern volatile uint8_t g_freq_index;
// Таймер для мигания
static volatile uint32_t g_blink_timer = 0;
// 0 = LED сейчас OFF, 1 = ON (для режима мигания)
static volatile uint8_t g_blink_on = 0; // Будет переключаться

void Interrupts_InitState(void);
void SysTick_Handler(void);
void EXTI0_IRQHandler(void);
void EXTI9_5_IRQHandler(void);

#endif