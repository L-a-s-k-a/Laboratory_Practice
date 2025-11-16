// В этом файле находятся все основные переменные и функции, используемые для основного и дополнительного заданий

#include "init.h"

// Общие состояния системы
volatile uint32_t leds_per_group = 1;  // Количество одновременно включаемых светодиодов
volatile uint32_t current_state = 0;   // 0 - процесс включения, 1 - процесс выключения
volatile uint32_t current_position = 0; // Текущая позиция для включения/выключения
volatile uint32_t button1_pressed = 0;
volatile uint32_t button2_pressed = 0;

// Переменные для дополнительного задания
volatile uint32_t selected_led = 0;
volatile uint32_t led_blink_mode[6] = {1, 1, 1, 1, 1, 1}; // 1: slow, 2: medium, 3: fast
volatile uint32_t system_time = 0;
volatile uint32_t button1_hold_time = 0;
volatile uint32_t button1_was_pressed = 0;
volatile uint32_t button1_long_press_handled = 0;
volatile uint32_t led_enabled;

// Периоды мигания
#define SLOW_BLINK_PERIOD 1000    // Медленное мигание: полный период 1000 тиков - 500 мс - включен, 500 мс - выключен
#define MEDIUM_BLINK_PERIOD 500  // Среднее мигание: полный период 500 тиков
#define FAST_BLINK_PERIOD 100    // Быстрое мигание: полный период 100 тиков
// Время зажатия кнопки
#define LONG_PRESS_TIME 1000      // 1000 тиков для длинного нажатия

// Общая функция задержки
void simple_delay(uint32_t delay) {
    for(volatile uint32_t i = 0; i < delay; i++);
}

#if defined(USE_REGISTERS_ONLY) // Реализация только через регистры

// Управление светодиодами
void set_led_state(uint32_t led_index, uint32_t state) {
    if (state) {
        // Устанавливаем соответствующий бит в ODR (PA2-PA7)
        // Включить светодиод
        *(volatile unsigned int*)(0x40020000UL + 0x14UL) |= (1UL << (led_index + 2));
    } else {
        // Выключить светодиод
        *(volatile unsigned int*)(0x40020000UL + 0x14UL) &= ~(1UL << (led_index + 2));
    }
}

uint32_t read_button1(void) {
    return *(volatile unsigned int*)(0x40020000UL + 0x10UL) & (1UL << 0);
}

uint32_t read_button2(void) {
    return *(volatile unsigned int*)(0x40020000UL + 0x10UL) & (1UL << 1);
}

#elif defined(USE_MACROS_ONLY) // Реализация через макросы и директивы

void set_led_state(uint32_t led_index, uint32_t state) {
    if (state) {
        GPIOA_ODR |= (1UL << (led_index + 2));
    } else {
        GPIOA_ODR &= ~(1UL << (led_index + 2));
    }
}

uint32_t read_button1(void) {
    return GPIOA_IDR & (1UL << 0);
}

uint32_t read_button2(void) {
    return GPIOA_IDR & (1UL << 1);
}

#elif defined(USE_CMSIS_ONLY)

void set_led_state(uint32_t led_index, uint32_t state) {
    if (state) {
        GPIOA->ODR |= (1UL << (led_index + 2));
    } else {
        GPIOA->ODR &= ~(1UL << (led_index + 2));
    }
}

uint32_t read_button1(void) {
    return GPIOA->IDR & GPIO_IDR_ID0;
}

uint32_t read_button2(void) {
    return GPIOA->IDR & GPIO_IDR_ID1;
}

// дополнительное задание
#elif defined(ADD_TASK)

void set_led_state(uint32_t led_index, uint32_t state) {
    if (state) {
        GPIOA->ODR |= (1UL << (led_index + 2));
    } else {
        GPIOA->ODR &= ~(1UL << (led_index + 2));
    }
}

uint32_t read_button1(void) {
    return GPIOA->IDR & GPIO_IDR_ID0;
}

uint32_t read_button2(void) {
    return GPIOA->IDR & GPIO_IDR_ID1;
}

#endif

#if defined(USE_REGISTERS_ONLY) || defined(USE_MACROS_ONLY) || defined(USE_CMSIS_ONLY)

// Общие функции для основных трех режимов

// Выключение всех светодиодов
void clear_all_leds(void) {
    for(int i = 0; i < 6; i++) {
        set_led_state(i, 0);
    }
}

// Включение всех светодиодов
void set_all_leds(void) {
    for(int i = 0; i < 6; i++) {
        set_led_state(i, 1);
    }
}

// Обновление количества светодиодов при нажатии первой кнопки
void handle_first_button(void) {
    // Увеличиваем количество светодиодов
    leds_per_group++;
    if (leds_per_group > 6) {
        leds_per_group = 1;
    }
}

// Обновление состояния светодиодов при нажатии второй кнопки
void handle_second_button(void) {
    // Специальный случай: если выбрано 6 светодиодов
    if (leds_per_group == 6) {
        static uint32_t all_leds_on = 0;
        if (all_leds_on) {
            clear_all_leds();
            all_leds_on = 0;
        } else {
            set_all_leds();
            all_leds_on = 1;
        }
        return;
    }
    
    // Обычный режим работы
    if (current_state == 0) {
        // Режим включения светодиодов
        for(uint32_t i = 0; i < leds_per_group; i++) {
            if (current_position + i < 6) {
                set_led_state(current_position + i, 1);
            }
        }
        
        current_position += leds_per_group;
        
        // Если включили все светодиоды, переходим в режим выключения
        if (current_position >= 6) {
            current_state = 1;
            current_position = 0;
        }
    } else {
        // Режим выключения светодиодов
        for(uint32_t i = 0; i < leds_per_group; i++) {
            if (current_position + i < 6) {
                set_led_state(current_position + i, 0);
            }
        }
        
        current_position += leds_per_group;
        
        // Если выключили все светодиоды, переходим в режим включения
        if (current_position >= 6) {
            current_state = 0;
            current_position = 0;
        }
    }
}

// Функция нажатия на обе кнопки
void handle_buttons(void) {
    // Обработка кнопки 1 (PA0) - изменение количества светодиодов
    if (read_button1()) {
        simple_delay(10000); // Антидребезг
        if (read_button1() && !button1_pressed) {
            button1_pressed = 1;
            handle_first_button();
        }
    } else {
        button1_pressed = 0;
    }
    
    // Обработка кнопки 2 (PA1) - управление светодиодами
    if (read_button2()) {
        simple_delay(10000); // Антидребезг
        if (read_button2() && !button2_pressed) {
            button2_pressed = 1;
            handle_second_button();
        }
    } else {
        button2_pressed = 0;
    }
}

#elif defined(ADD_TASK) // дополнительное задание

// Выключение всех светодиодов
void clear_all_leds(void) {
    for(int i = 0; i < 6; i++) {
        set_led_state(i, 0);
    }
}

// Включение всех светодиодов
void set_all_leds(void) {
    for(int i = 0; i < 6; i++) {
        set_led_state(i, 1);
    }
}

void handle_first_button(void) {
    static uint32_t last_button1_state = 0;
    uint32_t current_button1_state = read_button1();
    // Обнаружение нажатия кнопки
    if (current_button1_state && !last_button1_state) {
        // Кнопка только что нажата
        button1_was_pressed = 1;
        button1_hold_time = 0;
        button1_long_press_handled = 0;
    }
    // Обнаружение отпускания кнопки
    else if (!current_button1_state && last_button1_state) {
        // Кнопка только что отпущена
        if (button1_was_pressed && button1_hold_time < LONG_PRESS_TIME && !button1_long_press_handled) {
            // Короткое нажатие - меняем режим мигания выбранного светодиода (от 1 до 3)
            led_blink_mode[selected_led] = (led_blink_mode[selected_led] % 3) + 1;
        }
        button1_was_pressed = 0;
        button1_long_press_handled = 0;
    }
    // Кнопка удерживается
    else if (current_button1_state && button1_was_pressed) {
        button1_hold_time++;
        // Длинное нажатие
        if (button1_hold_time >= LONG_PRESS_TIME && !button1_long_press_handled) {
            selected_led = (selected_led + 1) % 6;
            button1_long_press_handled = 1;
        }
    }
    last_button1_state = current_button1_state;
}

void handle_second_button(void) {
    // Используем статические переменные для отслеживания состояния кнопки
    static uint32_t last_button2_state = 0;
    uint32_t current_button2_state = read_button2();
    // Обнаружение нажатия кнопки (передний фронт)
    if (current_button2_state && !last_button2_state) {
        current_position += leds_per_group;
        // Обработка нажатия второй кнопки
        if (current_state == 0) {
            // Если включили все светодиоды, переходим в режим выключения
            if (current_position >= 6) {
                current_state = 1;
                current_position = 0;
            }
        } else {        
            // Если выключили все светодиоды, переходим в режим включения
             if (current_position >= 6) {
                current_state = 0;
               current_position = 0;
            }
        }
    }
    
    last_button2_state = current_button2_state;
}

// Функция обновления состояния светодиодов для мигания
void update_leds(void) {
    for (int i = 0; i < 6; i++) {
        uint32_t mode = led_blink_mode[i];
        uint32_t state = 0;
        uint32_t period = 0;
        // Определяем период мигания в зависимости от режима
        switch (mode) {
            case 1: // Медленное мигание
                period = SLOW_BLINK_PERIOD;
                break;
            case 2: // Среднее мигание
                period = MEDIUM_BLINK_PERIOD;
                break;
            case 3: // Быстрое мигание
                period = FAST_BLINK_PERIOD;
                break;
            default: // Случай по умолчанию
                period = SLOW_BLINK_PERIOD;
                break;
        }
        // Светодиод горит первую половину периода и выключен вторую половину
        state = (system_time % period) < (period / 2);
        // Определяем, должен ли светодиод быть включен в текущем состоянии
        led_enabled = 0;
        if (current_state == 0){
            // Режим включения - светодиоды от 0 до current_position-1 включены
            led_enabled = (i < current_position);
        } else {
            // Режим выключения - светодиоды от current_position до 5 выключены
            led_enabled = (i >= current_position);
        }

        if (led_enabled) {
            set_led_state(i, state);
        } else {
            set_led_state(i, 0);
        }
    }
}

// запуск всех функций
void handle_buttons(void) {
    handle_first_button();
    handle_second_button();
    system_time++;
    update_leds();
}

#endif