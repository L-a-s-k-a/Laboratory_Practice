#include "init.h"

int main(void)
{
    GPIO_Ini(); // Инициализация GPIO

    int current_led = 0;       // Текущий активный светодиод
    int led_count = 1;         // Количество одновременно включённых светодиодов
    int button1_press = 0;     // Флаг для кнопки 1
    int button2_press = 0;     // Флаг для кнопки 2

    while (1)
    {
        // ===== Обработка кнопки 1: переключение активного светодиода =====
        if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0) { // Чтение состояния кнопки 1
            if (button1_press == 0) { // Если кнопка только что нажата
                button1_press = 1;
                current_led++;
                if (current_led > 2) {
                    current_led = 0; // Циклический переход
                }
            }
        } else {
            button1_press = 0; // Кнопка отпущена
        }

        // ===== Обработка кнопки 2: установка количества включённых светодиодов =====
        if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_10) != 0) { // Чтение состояния кнопки 2
            if (button2_press == 0) { // Если кнопка только что нажата
                button2_press = 1;
                led_count++;
                if (led_count > 3) {
                    led_count = 1; // Циклический переход
                }
            }
        } else {
            button2_press = 0; // Кнопка отпущена
        }

        // ===== Управление светодиодами =====
        if (led_count == 1) {
            // Один светодиод
            if (current_led == 0) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);  // Включить PB7
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14 | GPIO_BSRR_BR0); // Выключить остальные
            } else if (current_led == 1) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);  // Включить PB14
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7 | GPIO_BSRR_BR0);
            } else if (current_led == 2) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0); // Включить PB0
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7 | GPIO_BSRR_BR14);
            }
        } else if (led_count == 2) {
            // Два светодиода
            if (current_led == 0) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7 | GPIO_BSRR_BS14); // Включить PB7 и PB14
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0); // Выключить остальные
            } else if (current_led == 1) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14 | GPIO_BSRR_BS0); // Включить PB14 и PB0
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            } else if (current_led == 2) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0 | GPIO_BSRR_BS7); // Включить PB0 и PB7
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            }
        } else if (led_count == 3) {
            // Три светодиода
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7 | GPIO_BSRR_BS14 | GPIO_BSRR_BS0); // Включить все
        }

        // ===== Задержка для подавления дребезга =====
        for (volatile int i = 0; i < 100000; i++) {}
    }
}