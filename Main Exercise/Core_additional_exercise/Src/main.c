#include "Init.h"
#include "Init.c"

<<<<<<< HEAD
// Переменные
uint8_t blink_mode = 0;          // текущий режим мерцания
uint8_t current_led = 0;         // текущий активный светодиод (0-5 = PA5-PA10)
uint8_t prev_btn1 = 1;           // предыдущее состояние кнопок
uint8_t prev_btn2 = 1;
uint32_t blink_counter = 0;      // счетчик для мерцания
uint8_t led_state = 0;           // текущее состояние светодиода (0/1)

// Массив для хранения индивидуальных режимов мерцания для каждого светодиода
uint8_t led_blink_modes[6] = {0, 0, 0, 0, 0, 0};



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
        case 0: return 12; // Медленно 
        case 1: return 9;  // Средне 
        case 2: return 5;  // Быстро 
        
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

        // Кнопка 2 переключает активный светодиод
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
        for(uint32_t i = 0; i < 5000; i++);
    }
}
=======
uint8_t flag;



uint8_t btn1_state_viewer = 1;
uint8_t btn2_state_viewer = 1;
uint8_t leds_active_viewer = 0;
uint8_t led_count_viewer = 1;
// Переменные
int led_count = 1;     // сколько включается одновременно (1..6)
int active_leds = 0;   // сколько сейчас включено
uint8_t prev_btn1 = 1; // предыдущее состояние кнопок
uint8_t prev_btn2 = 1;


int main(void) {
    

    GPIO_Ini();
   
    while (1) {
        uint8_t btn1 = (GPIOA->IDR & GPIO_IDR_IDR0) >> 0;
        uint8_t btn2 = (GPIOA->IDR & GPIO_IDR_IDR1) >> 1;


        //Кнопка 1: задаёт количество одновременно включаемых светодиодов
        if (prev_btn1 == 1 && btn1 == 0) {
            led_count++;
            if (led_count > 6) led_count = 1;
        }
        prev_btn1 = btn1;


        //Кнопка 2: включает следующие led_count светодиодов
        if (prev_btn2 == 1 && btn2 == 0) {
            if (led_count == 6) {
                flag = 1;
                // если выбрано 6 — просто включаем/выключаем все
                if (active_leds == 0) {
                    for (int i = 5; i <= 10; i++) {
                        SET_BIT(GPIOA->BSRR, (1UL << i)); // включить
                    }
                    active_leds = 6;
                } else {
                    for (int i = 5; i <= 10; i++) {
                        SET_BIT(GPIOA->BRR, (1UL << i)); // выключить
                    }
                    active_leds = 0;
                }
            } else {
                flag = 0;
                // иначе поэтапное включение групп
                if (active_leds < 6) {
                    
                    for (int i = 0; i < led_count; i++) {
                        int pin = 5 + active_leds + i;
                        if (pin <= 10)
                            SET_BIT(GPIOA->BSRR, (1UL << pin));
                    }
                    active_leds += led_count;
                    if (active_leds > 6) active_leds = 6;
                } else {
                    // если уже все включены — выключаем все
                    for (int i = 5; i <= 10; i++)
                        SET_BIT(GPIOA->BRR, (1UL << i));
                    active_leds = 0;
                }
            }
        }
        prev_btn2 = btn2;

        btn1_state_viewer = btn1;
        btn2_state_viewer = btn2;
        leds_active_viewer = active_leds;
        led_count_viewer = led_count;

    
    }
}
>>>>>>> 1ec0a78e3ece6888e362f4c478aa3eb15ff44c45
