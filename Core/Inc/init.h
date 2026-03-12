// #endif 
#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f411xe.h"

// Пины светодиодов
#define LED1_PIN 4   // PA4
#define LED2_PIN 6   // PA6
#define LED3_PIN 7   // PA7
#define LED4_PIN 8   // PA8
#define LED5_PIN 9   // PA9
#define LED6_PIN 10  // PA10

// Пины кнопок
#define BUTTON1_PIN 1   // PB1
#define BUTTON2_PIN 2   // PB2
#define BUTTON3_PIN 3   // PB3

// Частоты для режима 1
extern const uint32_t mode1_periods[];    // 0.5 Гц, 1.7 Гц, 2.2 Гц

// Частоты для режима 2
extern const uint32_t mode2_periods[];    // 0.3 Гц, 0.8 Гц, 1.4 Гц

// Переменные состояния
extern volatile uint8_t current_mode;
extern volatile uint8_t mode1_freq_index;
extern volatile uint8_t mode2_freq_indices[6];
extern volatile uint8_t selected_led;

extern volatile uint8_t btn1_pressed;
extern volatile uint8_t btn2_pressed;
extern volatile uint8_t btn3_pressed;

extern volatile uint32_t sys_tick;
extern volatile uint32_t pair_timer;
extern volatile uint8_t current_pair;

// Для режима 2
extern volatile uint32_t led_timers[6];
extern volatile uint8_t led_states[6];
extern volatile uint8_t leds_enabled[6]; // 0 - выключен, 1 - включен и мерцает

// Прототипы функций
void LED_Init(void);
void BUTTON_Init(void);
void SysTick_Init(void);
void Set_LED(uint8_t led_index, uint8_t state);
void Process_Buttons(void);
void Handle_Buttons(void);
void Mode1_Process(void);
void Mode2_Process(void);
void SysTick_Handler(void);