#include "init.h"

// Глобальные переменные
volatile uint32_t button_press_start_time = 0;
volatile uint32_t button_press_duration = 0;
volatile uint8_t button_state = 0;
volatile uint32_t systick_counter = 0;

// Переменные для управления светодиодами
volatile uint8_t current_led = 0;        // Текущий горящий светодиод (0-5)
volatile uint8_t blink_enabled = 1;      // 1 - мерцание, 0 - постоянное свечение
volatile uint8_t blink_frequency = 0;    // 0: 0.4 Гц, 1: 1.1 Гц, 2: 1.9 Гц
volatile uint8_t leds_enabled = 1;       // 1 - светодиоды включены, 0 - выключены
volatile uint32_t last_blink_time = 0;   // Время последнего мигания

// Периоды мерцания для разных частот (в мс)
const uint32_t blink_periods[] = {2500, 909, 526}; // 0.4Гц, 1.1Гц, 1.9Гц

int main(void)
{
    Clock_Init_HSI_PLL_100MHz(); // настройка частоты тактирования на 100 МГц
    MCO_init(); // настройка вывода тактового сигнала на PC9
    SysTick_Init(); // инициализация системного таймера
    GPIO_Init(); // инициализируем порты

    // Изначально выключаем все светодиоды
    ClearAllLEDs();

    // Включаем первый светодиод
    SetLED(0);
    
    while (1)
    {
        UpdateBlink(); // Обновляем мерцание
        
        if (button_press_duration > 0) // если кнопка нажата
        {
            if (button_press_duration < 1000) // Функция 1: Кратковременное нажатие (< 1 сек)
            {
                if (leds_enabled) // если светодиоды включены
                {
                    current_led++; // переключаем на следующий светодиод
                    if (current_led > 5) // если горел последний светодиод
                    {
                        current_led = 0; // выбираем первый светодиод
                        leds_enabled = 0; // указываем что все светодиоды выключены
                        ClearAllLEDs(); // выключаем все светодиоды
                    }
                    else // если горел не последний светодиод
                    {
                        SetLED(current_led); // Включаем следующий светодиод
                    }
                }
                else // если светодиоды выключены
                {
                    leds_enabled = 1; // указываем что светодиоды включены
                    current_led = 0; // выбираем первый светодиод
                    SetLED(current_led); // и включаем его
                }
            }
            else if (button_press_duration >= 2000 && button_press_duration < 4000) // Функция 2: Удержание 2 секунды (изменение частоты мерцания)
            {
                if (blink_enabled) // Только если режим мерцания активен
                {
                    blink_frequency = (blink_frequency + 1) % 3; // выбираем следующий режим мерцания
                    
                    for (int i = 0; i < 3; i++) // Визуальное подтверждение - 3 быстрых мигания
                    {
                        ClearAllLEDs(); // выключаем все светодиоды
                        for (volatile uint32_t j = 0; j < 1000000; j++); // Задержка для 100 МГц - 10 миллисекунд
                        SetLED(current_led); // включаем этот светодиод
                        for (volatile uint32_t j = 0; j < 1000000; j++); // Задержка для 100 МГц - 10 миллисекунд
                    }
                }
            }
            else if (button_press_duration >= 4000) // Функция 3: Удержание 4 секунды (переключение режима мерцание/свечение)
            {
                blink_enabled = !blink_enabled; // переключаем режим мерцание/свечение
                
                if (!blink_enabled) // если выбран режим постоянного свечения
                {
                    SetLED(current_led); // включаем выбранный светодиод
                }
                
                for (int i = 0; i < 3; i++) // Визуальное подтверждение - 3 быстрых мигания
                {
                    ClearAllLEDs(); // выключаем все светодиоды
                    for (volatile uint32_t j = 0; j < 500000; j++); // Задержка для 100 МГц - 5 миллисекунд
                    SetLED(current_led); // включаем этот светодиод
                    for (volatile uint32_t j = 0; j < 500000; j++); // Задержка для 100 МГц - 5 миллисекунд
                }
            }
            
            button_press_duration = 0; // указываем что кнопка больше не нажата
            
            for (volatile uint32_t i = 0; i < 1500000; i++); // защита от дребезга
        }
        
        // Небольшая пауза в основном цикле
        for (volatile uint32_t i = 0; i < 200000; i++);
    }
}