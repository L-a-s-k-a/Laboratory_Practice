#include "stdint.h"
#include "stm32f4xx.h"
#include "stm32f411xe.h"
#include "stdlib.h"
// Глобальные переменные 
extern volatile uint32_t button_press_start_time;
extern volatile uint32_t button_press_duration;
extern volatile uint8_t button_state;
extern volatile uint32_t systick_counter;
// Переменные для управления светодиодами 
extern volatile uint8_t current_led;
extern volatile uint8_t blink_enabled;
extern volatile uint8_t blink_frequency;
extern volatile uint8_t leds_enabled;
extern volatile uint32_t last_blink_time;

// Периоды мерцания для разных частот (в мс)
extern const uint32_t blink_periods[];  // 0.4Гц, 1.1Гц, 1.9Гц

void Clock_Init_HSI_PLL_100MHz(void); // настраиваем тактирование на 100 МГц
void MCO_init(void); // инициализация MCO для проверки тактовой частоты
void GPIO_Init(void); // инициализация GPIO
void SysTick_Init(void); // Инициализация SysTick для 100 МГц
void SysTick_Handler(void); // Обработчик SysTick
uint32_t GetTick(void); // время с начала работы программы
void EXTI15_10_IRQHandler(void);
void SetLED(uint8_t led_num); // Включение конкретного светодиода
void ClearAllLEDs(void); // Выключение всех светодиодов
void UpdateBlink(void); // Обновление мерцания светодиодов