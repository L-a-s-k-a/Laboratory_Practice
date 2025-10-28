#include "../Inc/init.h"

// Счетчики для работы с кнопками
uint8_t button_pc12 = 0;
uint8_t button_pc13 = 0;
// Счетчики, показывающие количество горящих светодиодов
uint8_t LED = 0;
uint8_t LED_pc13 = 0;

int main(void)
{
    // Максимальное количество допустимых нажатий кнокпок
    uint8_t click_button_12 = 3;
    uint8_t click_button_13 = 1;
    // Инициализация портов
    GPIO_Ini();

    // Бесконечный цикл выполнения программы
    while (1)
    {
        // Счетчик для работы светодиодов
        if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_12) == 0)
        {
            button_pc12 = click_counter(button_pc12, click_button_12);
            while (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_12) == 0)
            {
            }
        }
        
        // Счетчик для изменения параметра работы порта PC12
        if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) == 0)
        {
            button_pc13 = click_counter(button_pc13, click_button_13);
            while (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) == 0)
            {
            }
        }

        for (int timer = 0; timer < 40000; timer++)
        {
        }
        LED = button_pc12;
        LED_pc13 = button_pc13;
        // LED_flickering(LED);  // Доп задание с мерцанием светодиодов
        turning_on_the_LED(LED);
        change_PC12(LED_pc13);
    }
}