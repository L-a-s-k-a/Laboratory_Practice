#include "init.h"
#include "extra.h" // Подключаем доп. задание

void main_delay(volatile uint32_t time) {
    while(time--) { __NOP(); }
}

int main(void) {
    // Настраиваем порты всеми тремя способами
    _init_CMSIS();      
    _init_myself_def(); 
    _init_mem();        

    // ====================================================================
     run_extra_task(); 
    // ====================================================================

   
    uint8_t led_count = 1; 
    uint8_t led_index = 0; 
    uint8_t btn1_was_pressed = 0;
    uint8_t btn2_was_pressed = 0;
    uint8_t all_leds_enabled = 1;

    while(1) {
        // первая кнопка
        if ((GPIOA->IDR & GPIO_IDR_ID5) == 0) { 
            main_delay(20000); // задержка
            if ((GPIOA->IDR & GPIO_IDR_ID5) == 0 && !btn1_was_pressed) { // проверяем на зажатие
                led_count = (led_count % 3) + 1; // циклический сдвиг на один
                if (led_count == 3) all_leds_enabled = 1; // для случая когда все три горят 
                btn1_was_pressed = 1;            
            }
        } else { btn1_was_pressed = 0; }
        // вторая кнопка
        if ((GPIOA->IDR & GPIO_IDR_ID6) == 0) {
            main_delay(20000); // задержка
            if ((GPIOA->IDR & GPIO_IDR_ID6) == 0 && !btn2_was_pressed) { // проверяем на зажатие
                if (led_count == 3) all_leds_enabled = !all_leds_enabled; // для случая когда все три горят 
                else led_index = (led_index + 1) % 3; // циклический сдвиг на один
                btn2_was_pressed = 1;            
            }
        } else { btn2_was_pressed = 0; }
        
        // гасим все 
        GPIOB->BSRR = (GPIO_BSRR_BR3 | GPIO_BSRR_BR4 | GPIO_BSRR_BR5);

        if (!(led_count == 3 && all_leds_enabled == 0)) {
            for (uint8_t i = 0; i < led_count; i++) { // количество одновременно включ. светидиодов
                uint8_t current_led = (led_index + i) % 3; // включаем следующий 
                if (current_led == 0) GPIOB->BSRR = GPIO_BSRR_BS3;
                if (current_led == 1) GPIOB->BSRR = GPIO_BSRR_BS4;
                if (current_led == 2) GPIOB->BSRR = GPIO_BSRR_BS5;
            }
        }
        main_delay(1000); 
    }
}
