#include "Init.h"
#include "Init.c"
#include "../../CMSIS/Devices/STM32F1xx/Inc/STM32f1xx.h"
#include "../../CMSIS/Devices/STM32F1xx/Inc/STM32f103xB/stm32f103xb.h"

uint8_t flag;
// Переменные
int led_count = 1;     // сколько включается одновременно (1..6)
int current_position = 0; // текущая позиция для следующей группы
uint8_t prev_btn1 = 1; // предыдущее состояние кнопок
uint8_t prev_btn2 = 1;

// Массив состояний ламп (0-выкл, 1-вкл)
uint8_t leds_state[6] = {0, 0, 0, 0, 0, 0};



void update_physical_leds(void) {
    // Синхронизируем физическое состояние с логическим
    for (int i = 0; i < 6; i++) {
        if (leds_state[i]) {
            GPIO_PORT_A_BSRR = (1UL << (5 + i)); ; // Включить лампу i
        } else {
            GPIO_PORT_A_BRR = (1UL << (5 + i));  // Выключить лампу i
        }
    }
}

void add_led_group(void) {
    // ВЫКЛЮЧАЕМ все лампы перед добавлением новой группы
    for (int i = 0; i < 6; i++) {
        leds_state[i] = 0;
    }
    
    // ВКЛЮЧАЕМ новую группу из led_count ламп, начиная с current_position
    for (int i = 0; i < led_count; i++) {
        int led_index = (current_position + i) % 6;
        leds_state[led_index] = 1; // Включаем лампу
    }
    
    // Обновляем позицию для следующей группы
    current_position = (current_position + led_count) % 6;
}

int main(void) {
    GPIO_Ini();
   
    while (1) {
        uint8_t btn1 = (GPIO_PORT_A_IDR & (1UL << 0)) ? 1 : 0;
        uint8_t btn2 = (GPIO_PORT_A_IDR & (1UL << 1)) ? 1 : 0;

        // Кнопка 1: ТОЛЬКО задаёт количество одновременно включаемых светодиодов
        if (prev_btn1 == 1 && btn1 == 0) {
            led_count++;
            if (led_count > 6) led_count = 1;
            // Только меняем значение, не меняем состояние ламп
        }
        prev_btn1 = btn1;

        // Кнопка 2: добавляет следующую группу светодиодов
        if (prev_btn2 == 1 && btn2 == 0) {
            if (led_count == 6) {
                flag = 1;
                // если выбрано 6 — просто включаем/выключаем все
                int all_on = 1;
                for (int i = 0; i < 6; i++) {
                    if (!leds_state[i]) {
                        all_on = 0;
                        break;
                    }
                }
                
                if (all_on) {
                    // Выключаем все
                    for (int i = 0; i < 6; i++) {
                        leds_state[i] = 0;
                    }
                    current_position = 0;
                } else {
                    // Включаем все
                    for (int i = 0; i < 6; i++) {
                        leds_state[i] = 1;
                    }
                }
            } else {
                flag = 0;
                // Добавляем новую группу ламп (старые выключаются автоматически)
                add_led_group();
            }
            
            // Обновляем физические светодиоды
            update_physical_leds();
        }
        prev_btn2 = btn2;

        // Задержка для антидребезга
        for(uint32_t i = 0; i < 100000; i++);
    }
}