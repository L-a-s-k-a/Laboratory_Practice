#include "it_handlers.h"

#define BTN_DELAY_FILTER 25
#define LONG_PRESS_DURATION 2000

extern int last_turned_led; // индекс диода для включения
extern int controlled_led;  // индекс управляемого диода
extern unsigned int led_tick_count[6];
extern uint32_t led_flick[6];
extern uint16_t led_period[3][3];
extern uint8_t index_in_range;  // индекс текущего диапазона частот
extern uint8_t index_of_range;  // индекс частоты в диапазоне
uint32_t B1_tick_count = 0, B2_tick_count = 0;
uint8_t B1_pressed = 0, B2_pressed = 0;

void B1_short_press() {
    if (++index_of_range == 3) index_of_range = 0;
    led_flick[controlled_led] = led_period[index_in_range][index_of_range];
}
void B2_short_press() {
    if (++index_in_range == 3) index_in_range = 0;
    led_flick[controlled_led] = led_period[index_in_range][index_of_range];
}

void B1_long_press() {
    if (++last_turned_led == 6) {
        last_turned_led = -1;
        CLEAR_BIT(GPIOC->ODR, 0b111111 << 7);
    } else if (last_turned_led >= 0) {
        SET_BIT(GPIOC->ODR, 1 << (7 + last_turned_led));
    }
}
void B2_long_press() {
    if (++controlled_led == 6) controlled_led = 0;
}

void EXTI15_10_IRQHanlder(void) {
    SET_BIT(EXTI->PR, EXTI_PR_PR13);
}

// Button 1
void EXTI0_IRQHandler(void) {
    SET_BIT(EXTI->PR, EXTI_PR_PR0);
    B1_pressed = READ_BIT(GPIOC->IDR, GPIO_IDR_ID0);
    if (!B1_pressed && B1_tick_count >= BTN_DELAY_FILTER) {
        // кнопку отпустили
        if (B1_tick_count >= LONG_PRESS_DURATION) B1_long_press();
        else B1_short_press();
    } else if (B1_pressed) {
        B1_tick_count = 0; // запускаем счётчик
    }
}
// Button 2
void EXTI3_IRQHandler(void) {
    SET_BIT(EXTI->PR, EXTI_PR_PR3);
    B2_pressed = READ_BIT(GPIOC->IDR, GPIO_IDR_ID3);
    if (!B2_pressed && B2_tick_count >= BTN_DELAY_FILTER) {
        // кнопку отпустили
        if (B2_tick_count >= LONG_PRESS_DURATION) B2_long_press();
        else B2_short_press();
    } else if (B2_pressed) {
        B2_tick_count = 0;
    }
}

// одновременное мигание для диодов с одинаковой частотой.
// обработка дребезга, доработать.
// вынести логику в отдельный файл.

void SysTick_Handler(void) {
    ++B1_tick_count;
    ++B2_tick_count;
    
    led_tick_count[0]++;
    led_tick_count[1]++;
    led_tick_count[2]++;
    led_tick_count[3]++;
    led_tick_count[4]++;
    led_tick_count[5]++;
}