#include "init.h"

uint8_t led_state[3] = {0, 0, 0};                                       //состояния светодиодов зелёный, синий, красный
uint8_t button_to_color[3] = {0, 1, 2};                                 //первая кнопка - зелёный и т.д.
uint8_t btn1_prev = 0, btn2_prev = 0, btn3_prev = 0, mode_prev = 0;     //переменные для переброса значений
uint8_t mode_count = 0;     //кнопарь

// Счётчики количества нажатий на кнопки
uint32_t btn1_press_count = 0;  // кнопка 1
uint32_t btn2_press_count = 0;  // кнопка 2
uint32_t btn3_press_count = 0;  //кнопка 3
uint32_t mode_press_count = 0;  //кнопка MODE
// Отдельные переменные для каждого светодиода иначе проблемы были с установкой на STMViewer
uint8_t led_green = 0;      //1 - горит, а больше и не надо
uint8_t led_blue = 0;       //1 - горит
uint8_t led_red = 0;        // 1 - горит
// Флаг для отладки 
uint8_t debug_flag = 0;         //отслеживания нажатий

// Функции
void disableAllLeds(void) {             //изначально выключая всё - светодиоды все закидываю в low уровень
    // Зеленые 
    GPIOA->BSRR = GPIO_BSRR_BR_5;
    GPIOC->BSRR = GPIO_BSRR_BR_5;
    // Синие 
    GPIOC_BSRR = BSRR_RESET_6;
    GPIOB_BSRR = BSRR_RESET_2;
    // Красные
    GPIOA_BSRR = BSRR_RESET_6;
    GPIOB_BSRR = BSRR_RESET_1;
}

void update_leds(void) {                //обновляю состояния диодов по массиву led_state (состояния)
    // Зелёные светодиоды 
    if (led_state[0]) {
        GPIOA->BSRR = GPIO_BSRR_BS_5;
        GPIOC->BSRR = GPIO_BSRR_BS_5;
    } else {
        GPIOA->BSRR = GPIO_BSRR_BR_5;
        GPIOC->BSRR = GPIO_BSRR_BR_5;
    }
    // Синие светодиоды 
    if (led_state[1]) {
        GPIOC_BSRR = BSRR_SET_6;
        GPIOB_BSRR = BSRR_SET_2;
    } else {
        GPIOC_BSRR = BSRR_RESET_6;
        GPIOB_BSRR = BSRR_RESET_2;
    }
    // Красные светодиоды 
    if (led_state[2]) {
        GPIOA_BSRR = BSRR_SET_6;
        GPIOB_BSRR = BSRR_SET_1;
    } else {
        GPIOA_BSRR = BSRR_RESET_6;
        GPIOB_BSRR = BSRR_RESET_1;
    }
    
    // переменную синхронизирую 
    led_green = led_state[0];
    led_blue = led_state[1];
    led_red = led_state[2];
}

//читаю кнопку по фронту отпущена -> нажата
//указываю на регистр, маска кнопки, указываю на переменную и возвращаю с неё значение
uint8_t readButton(uint32_t *idr, uint32_t mask, uint8_t* prevState) {    
    //считываю состояние с регистра, получаю бит с кнопки, инвертирую для подтверждения нажатия 
    uint8_t pressed = !(*idr & mask);           
    uint8_t result = 0;                         
    if (pressed && !(*prevState)) result = 1;   // если кнопка нажата сейчас и была отпущена, то фронт есть
    *prevState = pressed; //запомнили состояние кнопки для следующей проходки
    return result;
}

void GPIO_Init_All(void) {
    // Включение тактирования
    RCC_AHB1ENR |= 0x07;
    // Зелёные светодиоды 
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER5_0);    // PA5 - зелёный диод на GPIOA
    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODER5_1);
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER5_0);    // PC5 - зелёный диод на GPIOC
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODER5_1);
    // Красные светодиоды 
    GPIOA_MODER |= MODER_PA6_OUTPUT;        // PA6 - красный диод на GPIOA
    GPIOB_MODER |= MODER_PB1_OUTPUT;        // PB1 - красный диод на GPIOB
    // Синие светодиоды 
    GPIOC_MODER |= MODER_PC6_OUTPUT;        // PC6 - синий диод на GPIOC
    GPIOB_MODER |= MODER_PB2_OUTPUT;        // PB2 - синий диод на GPIOB
    // Кнопки
    GPIOC_PUPDR |= PUPDR_PC13_PULLUP;           // кнопа1 - PC13 
    GPIOB_PUPDR |= PUPDR_PB12_PULLUP;           // кнопа2 - PB12 
    // кнопа3 - PC4 
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODER4_0);
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODER4_1);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR4_0);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR4_1);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR4_0);
    // MODE - PA11 
    GPIOA_PUPDR |= PUPDR_PA11_PULLUP;
    disableAllLeds();
}

/*
#include "init.h"

// Глобальные переменные
extern uint32_t blink_delays[Number_of_freq];               //задержки
extern uint8_t current_freq_index[Number_of_elements];      //индексы пар
extern uint8_t current_order_index;                         //выбранная пара
extern uint8_t state_array[Number_of_elements];             //флаги проверки
extern uint32_t delay_counters[Number_of_elements];         //задержки
extern uint8_t led_visible[Number_of_elements];             //видимость светодиодов
extern uint8_t btn1_prev, btn2_prev, btn3_prev, mode_prev;  // предыдущие состояния кнопок

// Функции
void disableAllLeds(void) {             //изначально выключаю все светодиоды, все закидываю в low уровень
    // Выключить все светодиоды
    GPIOA_BSRR = GPIO_BSRR_RESET(GPIO_PIN_5);       
    GPIOC_BSRR = GPIO_BSRR_RESET(GPIO_PIN_5);       
    GPIOA_BSRR = GPIO_BSRR_RESET(GPIO_PIN_6);
    GPIOB_BSRR = GPIO_BSRR_RESET(GPIO_PIN_1);
    GPIOC_BSRR = GPIO_BSRR_RESET(GPIO_PIN_6);
    GPIOB_BSRR = GPIO_BSRR_RESET(GPIO_PIN_2);
}




void update_leds(void) {                //обновляю состояния диодов по массиву led_state (состояния)

    
    // Первая пара зеленые: PA5 и PC5
    if (state_array[0] && led_visible[0]) {
        GPIOA_BSRR = GPIO_BSRR_SET(GPIO_PIN_5);
        GPIOC_BSRR = GPIO_BSRR_SET(GPIO_PIN_5);
    } else {
        GPIOA_BSRR = GPIO_BSRR_RESET(GPIO_PIN_5);
        GPIOC_BSRR = GPIO_BSRR_RESET(GPIO_PIN_5);
    }
    
    // Вторая пара синие: PC6 и PB2
    if (state_array[1] && led_visible[1]) {
        GPIOC_BSRR = GPIO_BSRR_SET(GPIO_PIN_6);
        GPIOB_BSRR = GPIO_BSRR_SET(GPIO_PIN_2);
    } else {
        GPIOC_BSRR = GPIO_BSRR_RESET(GPIO_PIN_6);
        GPIOB_BSRR = GPIO_BSRR_RESET(GPIO_PIN_2);
    }
    
    // Третья пара красные: PA6 и PB1
    if (state_array[2] && led_visible[2]) {
        GPIOA_BSRR = GPIO_BSRR_SET(GPIO_PIN_6);
        GPIOB_BSRR = GPIO_BSRR_SET(GPIO_PIN_1);
    } else {
        GPIOA_BSRR = GPIO_BSRR_RESET(GPIO_PIN_6);
        GPIOB_BSRR = GPIO_BSRR_RESET(GPIO_PIN_1);
    }
}


uint8_t readButton(uint32_t *idr, uint32_t mask, uint8_t* prevState) {     //указываю не регистр, маска кнопки, указываю на переменную и возвращаю с неё значение
    uint8_t pressed = !(*idr & mask);           //считываю состояние с регистра, получаю бит с кнопки, инвертирую для подтверждения нажатия
    uint8_t result = 0;                         
    if (pressed && !(*prevState)) result = 1;   // если кнопка нажата сейчас и была отпущена, то фронт есть
    *prevState = pressed; //запомнили состояние кнопки для следующей проходки
    return result;
}


void GPIO_Init_All(void) {
    // Включение тактирования портов
    RCC_AHB1ENR |= (RCC_GPIOA_EN | RCC_GPIOB_EN | RCC_GPIOC_EN);

    // режим OUTPUT
    // Зелёные светодиоды (PA5, PC5)
    GPIOA_MODER |= MODER_PA5_OUTPUT;
    GPIOC_MODER |= MODER_PC5_OUTPUT;
    
    // Синие светодиоды (PC6, PB2)
    GPIOC_MODER |= MODER_PC6_OUTPUT;
    GPIOB_MODER |= MODER_PB2_OUTPUT;
    
    // Красные светодиоды (PA6, PB1)
    GPIOA_MODER |= MODER_PA6_OUTPUT;
    GPIOB_MODER |= MODER_PB1_OUTPUT;

    //режим INPUT с подтяжкой
    // BTN1 - PC13 (вход с подтяжкой)
    GPIOC_PUPDR |= PUPDR_PC13_PULLUP;
    
    // BTN2 - PB12 (вход с подтяжкой)
    GPIOB_PUPDR |= PUPDR_PB12_PULLUP;
    
    // BTN3 - PC4 (вход с подтяжкой)
    GPIOC_PUPDR |= PUPDR_PC4_PULLUP;
    
    // MODE - PA11 (вход с подтяжкой)
    GPIOA_PUPDR |= PUPDR_PA11_PULLUP;
    
    disableAllLeds();
}

*/