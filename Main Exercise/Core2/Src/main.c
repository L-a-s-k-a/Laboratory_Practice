#include "Init.h"
#include "Init.c"

// Переменные
uint8_t blink_mode = 0;          // текущий режим мерцания
uint8_t current_led = 0;         // текущий активный светодиод (0-5 = PA5-PA10)
uint8_t prev_btn1 = 1;           // предыдущее состояние кнопок
uint8_t prev_btn2 = 1;
uint32_t blink_counter = 0;      // счетчик для мерцания
uint8_t led_state = 0;           // текущее состояние светодиода (0/1)

// Массив для хранения индивидуальных режимов мерцания для каждого светодиода
uint8_t led_blink_modes[6] = {0, 0, 0, 0, 0, 0};

// Адреса регистров
// #define GPIOA_IDR  (*(uint32_t*)(0x40010808))
// #define GPIOA_BSRR (*(uint32_t*)(0x40010810))
// #define GPIOA_BRR  (*(uint32_t*)(0x40010814))

void update_physical_leds(void) {
    // Сначала выключаем ВСЕ светодиоды
    for (int i = 5; i <= 10; i++) {
        GPIOA_BRR = (1UL << i);
    }
    
    // Если светодиод должен гореть
    if (led_state) {
        GPIOA_BSRR = (1UL << (5 + current_led));
    }
}

uint32_t get_blink_delay(uint8_t mode) {
    // Возвращает задержку для мерцания в зависимости от режима
    switch(mode) {
        case 0: return 12; // Медленно Гц
        case 1: return 6;  // Средне Гц
        case 2: return 2;  // Быстро Гц
        
    }
}

int main(void) {
    GPIO_Ini();
   
    while (1) {
        uint8_t btn1 = (GPIOA_IDR & (1UL << 0)) ? 1 : 0;
        uint8_t btn2 = (GPIOA_IDR & (1UL << 1)) ? 1 : 0;

        // Кнопка 1: устанавливает режим мерцания для ТЕКУЩЕГО светодиода
        if (prev_btn1 == 1 && btn1 == 0) {
            // Увеличиваем режим мерцания для текущего светодиода
            led_blink_modes[current_led]++;
            if (led_blink_modes[current_led] > 2) {
                led_blink_modes[current_led] = 0;
            }
            
            // Обновляем текущий режим для отображения
            blink_mode = led_blink_modes[current_led];
            
            // Сбрасываем счетчик для немедленного применения новой частоты
            blink_counter = 0;
        }
        prev_btn1 = btn1;

        // Кнопка 2: переключает активный светодиод
        if (prev_btn2 == 1 && btn2 == 0) {
            current_led++;
            if (current_led > 5) current_led = 0;
            
            // При переключении светодиода загружаем его режим мерцания
            blink_mode = led_blink_modes[current_led];
            
            // Сбрасываем счетчик мерцания
            blink_counter = 0;
            led_state = 1; // Включаем новый светодиод сразу
            
            // Обновляем физические светодиоды
            update_physical_leds();
        }
        prev_btn2 = btn2;

        // Логика мерцания текущего светодиода
        blink_counter++;
        uint32_t blink_delay = get_blink_delay(blink_mode);
        
        if (blink_counter >= blink_delay) {
            blink_counter = 0;
            led_state = !led_state; // Переключаем состояние
            
            // Обновляем только текущий светодиод
            if (led_state) {
                GPIOA_BSRR = (1UL << (5 + current_led));
            } else {
                GPIOA_BRR = (1UL << (5 + current_led));
            }
        }

        // Короткая задержка для плавности
        for(volatile uint32_t i = 0; i < 5000; i++);
    }
}