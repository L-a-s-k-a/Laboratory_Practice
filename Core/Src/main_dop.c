/*#include "init.h"    
#include <stdint.h> 

#define MAX_LIGHTS          3      
#define DEBOUNCE_TIME_MS    20    
#define LONG_PRESS_MS       500    
#define BUTTON_POLL_MS      10     

// скорости мигания (в миллисекундах)
#define BLINK_SPEED_SLOW    1   
#define BLINK_SPEED_MEDIUM  10  
#define BLINK_SPEED_FAST    20  

// о светодиоде
typedef struct {
    uint8_t is_on;  
} light_t;

// функция для кнопки
typedef void (*button_action)(void);

// информация о кнопке
typedef struct {
    uint8_t is_pressed;         // нажата сейчас
    uint32_t press_start_time;  // когда нажали
} button_t;

// скорости мигания
typedef enum {
    SPEED_SLOW = 0,
    SPEED_MEDIUM,
    SPEED_FAST
} blink_speed_t;

// глобальные переменные
volatile uint32_t system_tick = 0;  // счетчик времени
light_t lights[MAX_LIGHTS];         // все светодиоды
button_t buttons[4];                // все кнопки
uint8_t light_stack[MAX_LIGHTS];    // стек включенных светодиодов
uint8_t stack_top = 0;              // сколько светодиодов включено
uint8_t blinking_light_id = 0;      // какой светодиод мигает
blink_speed_t blink_speed = SPEED_SLOW; // скорость мигания
uint8_t blink_state = 1;            // состояние мигания
uint32_t blink_counter = 0;         // счетчик для мигания

// отображение функций на кнопки (0:добавить, 1:скорость, 2:мигающий)
static uint8_t button_func_mapping[3] = {0, 1, 2};

// функции кнопок 
static void handle_button1_action(void);  // добавить светодиод
static void handle_button2_action(void);  // изменить скорость
static void handle_button3_action(void);  // сменить мигающий
static void handle_button8_short(void);   // убрать светодиод
static void handle_button8_long(void);    // поменять функции (без индикации)

// работа со светодиодами
static void update_lights_from_stack(void);
static void push_light_to_stack(uint8_t light_id);
static void pop_light_from_stack(void);

// работа с кнопками
static void process_buttons(void);

// мигание
static void update_blinking(void);

// обновить светодиоды по стеку
static void update_lights_from_stack(void) {
    // все выключаем
    for (int i = 0; i < MAX_LIGHTS; i++) {
        led_off(i);
    }
    
    // включаем по стеку
    for (uint8_t i = 0; i < stack_top; i++) {
        uint8_t light_id = light_stack[i];
        if (light_id == blinking_light_id && !blink_state) {
            continue; // пропускаем мигающий светодиод в выключенной фазе
        }
        led_on(light_id);
    }
}

// добавить в стек
static void push_light_to_stack(uint8_t light_id) {
    if (stack_top < MAX_LIGHTS) {
        light_stack[stack_top] = light_id;
        stack_top++;
        update_lights_from_stack();
    }
}

// убрать из стека
static void pop_light_from_stack(void) {
    if (stack_top > 0) {
        stack_top--;
        // Если убран мигающий светодиод, сбрасываем мигание
        if (stack_top > 0) {
            blink_state = 1;
            blink_counter = 0;
        }
        update_lights_from_stack();
    }
}

// кнопка 1: добавить светодиод
static void handle_button1_action(void) {
    if (stack_top < MAX_LIGHTS) {
        // ищем свободный светодиод
        for (uint8_t i = 0; i < MAX_LIGHTS; i++) {
            uint8_t found = 0;
            for (uint8_t j = 0; j < stack_top; j++) {
                if (light_stack[j] == i) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                push_light_to_stack(i);
                break;
            }
        }
    }
}

// кнопка 2: изменить скорость
static void handle_button2_action(void) {
    blink_speed = (blink_speed + 1) % 3;
    blink_state = 1;
    blink_counter = 0;
    update_lights_from_stack();
}

// кнопка 3: сменить мигающий
static void handle_button3_action(void) {
    blinking_light_id = (blinking_light_id + 1) % MAX_LIGHTS;
    blink_state = 1;
    blink_counter = 0;
    update_lights_from_stack();
}

// кнопка 8 коротко: убрать светодиод
static void handle_button8_short(void) {
    if (stack_top > 0) {
        pop_light_from_stack();
    }
}

// кнопка 8 долго: поменять функции (БЕЗ ИНДИКАЦИИ)
static void handle_button8_long(void) {
    // просто меняем функции - никаких светодиодов не включаем
    uint8_t temp = button_func_mapping[2];
    button_func_mapping[2] = button_func_mapping[1];
    button_func_mapping[1] = button_func_mapping[0];
    button_func_mapping[0] = temp;
    // ВСЁ! Никакой индикации, никаких светодиодов
}

// проверить все кнопки
static void process_buttons(void) {
    // функции для кнопок
    static button_action button_funcs[3] = {
        handle_button1_action,
        handle_button2_action,
        handle_button3_action
    };
    
    // проверяем 4 кнопки
    for (int i = 0; i < 4; i++) {
        uint8_t state = read_button_state(i);
        button_t *btn = &buttons[i];
        
        // только что нажали
        if (state && !btn->is_pressed) {
            btn->is_pressed = 1;
            btn->press_start_time = system_tick;
        } 
        // только что отпустили
        else if (!state && btn->is_pressed) {
            btn->is_pressed = 0;
            uint32_t duration = system_tick - btn->press_start_time;
            
            // игнорируем дребезг
            if (duration >= DEBOUNCE_TIME_MS) {
                // кнопка b8 (индекс 3)
                if (i == 3) {
                    if (duration >= LONG_PRESS_MS) {
                        handle_button8_long();  // долгое нажатие - смена функций
                    } else {
                        handle_button8_short(); // короткое нажатие - убрать светодиод
                    }
                } 
                // кнопки b1-b3 (индексы 0-2)
                else if (duration < LONG_PRESS_MS && i < 3) {
                    uint8_t func_idx = button_func_mapping[i];
                    if (func_idx < 3) {
                        button_funcs[func_idx](); // выполняем назначенную функцию
                    }
                }
            }
        }
    }
}

// обновить мигание
static void update_blinking(void) {
    if (stack_top == 0) return;
    
    // проверяем есть ли мигающий светодиод в стеке
    uint8_t found = 0;
    for (uint8_t i = 0; i < stack_top; i++) {
        if (light_stack[i] == blinking_light_id) {
            found = 1;
            break;
        }
    }
    if (!found) return;
    
    // обновляем счетчик
    blink_counter++;
    
    // выбираем скорость в зависимости от настроек
    uint32_t period_ms = 0;
    switch (blink_speed) {
        case SPEED_SLOW: period_ms = BLINK_SPEED_SLOW; break;
        case SPEED_MEDIUM: period_ms = BLINK_SPEED_MEDIUM; break;
        case SPEED_FAST: period_ms = BLINK_SPEED_FAST; break;
    }
    
    // если прошло достаточно времени - меняем состояние
    if (blink_counter >= period_ms) {
        blink_counter = 0;
        blink_state = !blink_state;
        update_lights_from_stack();
    }
}

int main(void) {
    // инициализация периферии
    GPIO_Init_with_memory_for_led5();
    GPIO_Init_with_CMSIS();
    
    // инициализация переменных
    for (int i = 0; i < MAX_LIGHTS; i++) {
        lights[i].is_on = 0;
        led_off(i);
    }
    
    for (int i = 0; i < 4; i++) {
        buttons[i].is_pressed = 0;
        buttons[i].press_start_time = 0;
    }
    
    // начальные настройки
    stack_top = 0;
    blinking_light_id = 0;
    blink_speed = SPEED_SLOW;
    blink_state = 1;
    blink_counter = 0;
    
    // сброс отображения функций
    button_func_mapping[0] = 0;
    button_func_mapping[1] = 1;
    button_func_mapping[2] = 2;
    
    // переменные для управления временем
    uint32_t last_button_poll = 0;
    uint32_t last_blink_update = 0;
    
    while (1) {
        uint32_t current_time = system_tick;
        
        // опрос кнопок
        if (current_time - last_button_poll >= BUTTON_POLL_MS) {
            last_button_poll = current_time;
            process_buttons();
        }
        
        // обновление мигания
        if (current_time - last_blink_update >= 1) {
            last_blink_update = current_time;
            update_blinking();
        }
        
        // задержка для управления частотой цикла
        ms_delay(1);
        system_tick++;
    }
    
    return 0;
}*/