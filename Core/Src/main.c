#include "init.h"
int main(void) {
    GPIO_Init_All(); // иницилизирую все порты
    // Инициализация предыдущих состояний (проверка для СДВиГа групп)
    btn1_prev = !((uint32_t)GPIOC_IDR & GPIO_PIN_13); //считать текущее состояние BTN1 и инвертируем
    btn2_prev = !((uint32_t)GPIOB_IDR & GPIO_PIN_12);
    btn3_prev = !(GPIOC->IDR & GPIO_IDR_ID4);
    mode_prev = !((uint32_t)GPIOA_IDR & GPIO_PIN_11);
    update_leds(); // обновление состояния светодиодов
    while (1) {
        // Обработка кнопки MODE
        if (readButton((uint32_t *)&GPIOA_IDR, GPIO_PIN_11, &mode_prev)) {  //обработка с кнопке
            mode_press_count++;  // Увеличиваем счётчик нажатий MODE
            debug_flag = 1;      // Флаг для STMViewer
            uint8_t tmp = button_to_color[2];
            button_to_color[2] = button_to_color[1];
            button_to_color[1] = button_to_color[0];
            button_to_color[0] = tmp;
            mode_count++; 
            if (mode_count % 3 == 0) {
                button_to_color[0] = 0;
                button_to_color[1] = 1;
                button_to_color[2] = 2;
            }
        }
        
        // Обработка BTN1 переключаю цвет, который к этой кнопке привязан
        if (readButton((uint32_t *)&GPIOC_IDR, GPIO_PIN_13, &btn1_prev)) {
            btn1_press_count++;  // Увеличиваем счётчик нажатий кнопки 1
            debug_flag = 2;      // Флаг для STMViewer
            
            uint8_t color = button_to_color[0];
            led_state[color] = !led_state[color];
            
            update_leds();
        }
        // Обработка BTN2
        if (readButton((uint32_t *)&GPIOB_IDR, GPIO_PIN_12, &btn2_prev)) {
            btn2_press_count++;  // Увеличиваем счётчик нажатий кнопки 2
            debug_flag = 3;      // Флаг для STMViewer
            
            uint8_t color = button_to_color[1];
            led_state[color] = !led_state[color];
            
            update_leds();
        }
        // Обработка BTN3
        if (readButton(&GPIOC->IDR, GPIO_IDR_ID4, &btn3_prev)) {
            btn3_press_count++;  // Увеличиваем счётчик нажатий кнопки 3
            debug_flag = 4;      // Флаг для STMViewer
            
            uint8_t color = button_to_color[2];
            led_state[color] = !led_state[color];
            
            update_leds();
        }
        debug_flag = 0;  //сбрасываю флаг после обработки всей
        for (int timer = 0; timer < 50000; timer++);
    }
    return 0;
}



/*
#include "init.h"


int main(void) {
    GPIO_Init_All(); // иницилизирую все порты
    
    //Предыдущие состояния кнопок 
    btn1_prev = !(GPIOC_IDR & GPIO_PIN_13);
    btn2_prev = !(GPIOB_IDR & GPIO_PIN_12);
    btn3_prev = !(GPIOC_IDR & GPIO_PIN_4);
    mode_prev = !(GPIOA_IDR & GPIO_PIN_11);
    
    while (1) {
        // Обработка BTN1 - переключение частоты мигания
        if (readButton(&GPIOC_IDR, GPIO_PIN_13, &btn1_prev)) {
            current_freq_index[current_order_index]++;
            if (current_freq_index[current_order_index] >= Number_of_freq) {
                current_freq_index[current_order_index] = 0;
            }
        }

        // Обработка BTN2 - переключение выбранной пары светодиодов
        if (readButton(&GPIOB_IDR, GPIO_PIN_12, &btn2_prev)) {
            current_order_index++;
            if (current_order_index >= Number_of_elements) {
                current_order_index = 0;
            }
        }
        
        // Обработка BTN3 - включить мигание выбранной пары
        if (readButton(&GPIOC_IDR, GPIO_PIN_4, &btn3_prev)) {
            state_array[current_order_index] = 1;
            led_visible[current_order_index] = 1;
            update_states();
        }
        
        // Обработка MODE - выключить мигание выбранной пары
        if (readButton(&GPIOA_IDR, GPIO_PIN_11, &mode_prev)) {
            state_array[current_order_index] = 0;
            led_visible[current_order_index] = 0;
            update_states();
        }

        // Основной цикл мигания светодиодов
        for (uint8_t i = 0; i < Number_of_elements; i++) {
            if (state_array[i]) {
                delay_counters[i]++;
                
                if (delay_counters[i] >= blink_delays[current_freq_index[i]]) {
                    delay_counters[i] = 0;
                    led_visible[i] = !led_visible[i];
                    update_states();
                }
            }
        }
    }
    return 0;
}

*/