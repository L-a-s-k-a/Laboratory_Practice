#include "init.h"

// Частоты для режима 1
const uint32_t mode1_periods[] = {1000, 588, 455};    // 0.5 Гц, 1.7 Гц, 2.2 Гц

// Частоты для режима 2
const uint32_t mode2_periods[] = {1667, 625, 357};    // 0.3 Гц, 0.8 Гц, 1.4 Гц

// Переменные состояния
volatile uint8_t current_mode = 0;
volatile uint8_t mode1_freq_index = 0;
volatile uint8_t mode2_freq_indices[6] = {0, 0, 0, 0, 0, 0};
volatile uint8_t selected_led = 0;

volatile uint8_t btn1_pressed = 0;
volatile uint8_t btn2_pressed = 0;
volatile uint8_t btn3_pressed = 0;

volatile uint32_t sys_tick = 0;
volatile uint32_t pair_timer = 0;
volatile uint8_t current_pair = 0;

// Для режима 2
volatile uint32_t led_timers[6] = {0};
volatile uint8_t led_states[6] = {0};
volatile uint8_t leds_enabled[6] = {0, 0, 0, 0, 0, 0}; // 0 - выключен, 1 - включен и мерцает

int main(void) {
    // Инициализация
    LED_Init();
    BUTTON_Init();
    SysTick_Init();
    
    // Инициализация таймеров
    pair_timer = sys_tick;
    for(int i = 0; i < 6; i++) {
        led_timers[i] = sys_tick;
    }
    
    // Включаем прерывания
    __enable_irq();
    
    // Основной цикл
    while(1) {
        Process_Buttons();
        Handle_Buttons();
        
        if(current_mode == 0) {
            Mode1_Process();
        } else {
            Mode2_Process();
        }
        
        for(volatile int i = 0; i < 1000; i++);
    }
}


