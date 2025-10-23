#include "Init.h"
#include "Init.c"

uint8_t flag;
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

    
    }
}
