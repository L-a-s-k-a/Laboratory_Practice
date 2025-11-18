#include "../Inc/init.h"
#include <stdint.h>

uint8_t led_state = 0;       // LED Переменные состояния
uint8_t button1_pressed = 0; // Переменная состояния кнопки 1
uint8_t button2_pressed = 0; // Переменная состояния кнопки 2
uint8_t mode = 0;            // Режимы работы кнопки 2: 0: светодиод включен, 1: светодиод выключен

uint8_t button1_flag;
uint8_t button2_flag;

#define DEBOUNCE_DELAY 200 // Время задержки стабилизации изображения, в миллисекундах

void delay(uint32_t s)
{
    while (s--);
}

int main(void)
{
    GPIO_Ini_1(); // Инициализировать порты GPIO

    while (1)
    {
        // Проверьте состояние кнопки 1 (PC12)

        button1_flag = !button1_pressed;
        button2_flag = !button2_pressed;
        // if ((READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN12) != 0) && !button1_pressed)
        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_ID12) != 0) && !button1_pressed)
        {                          // При нажатии кнопки 1 PC12 равен 0.
            delay(DEBOUNCE_DELAY); // Задержка стабилизации изображения
            if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID12) != 0)
            {
                mode ^= 1;           // Переключить режим
                button1_pressed = 1; // Кнопка обновления теперь нажата.
            }
        }
        else if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID12) == 0)
        {
            button1_pressed = 0; // Сброс состояния кнопки
        }

        // Проверьте состояние кнопки 2 (PC13)
        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) == 0) && !button2_pressed)
        {                          // При нажатии кнопки 2 PC13 равен 0.
            delay(DEBOUNCE_DELAY); // Задержка стабилизации изображения
            if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) == 0)
            {
                // Переключение состояния светодиода в зависимости от режима
                if (mode == 0)
                { // Открытый режим
                    if (led_state < 3)
                    {
                        if (led_state == 0)
                        {
                            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); // Загорается светодиод LED1
                        }
                        else if (led_state == 1)
                        {
                            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); // Загорается светодиод LED2
                        }
                        else if (led_state == 2)
                        {
                            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET); // Загорается светодиод LED3
                        }
                        led_state++; // Обновить статус
                    }
                }
                else
                { // Выключенный режим
                    if (led_state > 0)
                    {
                        if (led_state == 1)
                        {
                            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0); // LED1 выключен
                        }
                        else if (led_state == 2)
                        {
                            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); // Выключить LED2
                        }
                        else if (led_state == 3)
                        {
                            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14); // Выключить LED3
                        }
                        led_state--; // Обновить статус
                    }
                }

                button2_pressed = 1; // Кнопка обновления теперь нажата.
            }
        }
        else if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0)
        {
            button2_pressed = 0; // Сброс состояния кнопки
        }

        delay(20000); // Задержка для предотвращения быстрого зацикливания
    }
}

