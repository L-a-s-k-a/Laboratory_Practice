#include <init.h>

void dop_zadanie(void)
{
    __dop_zadanie_init();

    uint32_t T = 8000;     // Период ШИМ
    int32_t bright = 4000; // Яркость

    uint32_t step_speed = 1;      // Шаг изменения яркости
    uint8_t but_pb8_press = 0;    // Нажатие кнопки PB8

    while (1)
    { 
        // Управление яркостью
        if (bright > 0)
        {
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS7); // Включаем светодиод PC7
            delay(bright);                       // Время во включенном состоянии
        }

        if (bright < T)
        {
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR7); // Выключаем светодиод PC7
            delay(T - bright);                   // Время в выключенном состоянии
        }

        // Кнопка PB6: Увеличение яркости
        if ((GPIOB->IDR & GPIO_IDR_ID6) == 0)
        {
            bright += step_speed; // Увеличиваем яркость на величину текущей скорости
            if (bright > T)
            {
                bright = T; // Ограничение максимума
            }
        }

        // Кнопка PB9: Уменьшение яркости
        if ((GPIOB->IDR & GPIO_IDR_ID9) == 0)
        {
            bright -= step_speed; // Уменьшаем яркость
            if (bright < 0)
            {
                bright = 0; // Ограничение минимума
            }
        }

        // Кнопка PB8): Изменение скорости
        if ((GPIOB->IDR & GPIO_IDR_ID8) == 0)
        {
            if (!but_pb8_press)
            {
                but_pb8_press = 1;      // Запоминаем, что кнопка удерживается

                if (step_speed == 1)
                {
                    step_speed = 3;     // Увеличили скорость после первого нажатия
                }
                else if (step_speed == 3)
                {
                    step_speed = 6;     // Увеличили скорость после второго нажатия
                }
                else if (step_speed == 6)
                {
                    step_speed = 12;    // Увеличили скорость после третьего нажатия
                }
                else
                {
                    step_speed = 1;     // Вернулись к первоначальной скорости после четвертого нажатия
                }
                
            }
        }
        else
        {
            but_pb8_press = 0; // Кнопка отпущена
        }
    }
}