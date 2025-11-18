#include "Interrupt.h"
#include "Init.h"
#include "stm32f4xx.h"

volatile uint32_t system_time_ms = 0;

/* Переменные состояния диодов (флаги) для MCUViewer */
volatile uint8_t led1_state = 0;
volatile uint8_t led2_state = 0;
volatile uint8_t led3_state = 0;
volatile uint8_t led4_state = 0;
volatile uint8_t led5_state = 0;
volatile uint8_t led6_state = 0;

/* Флаги кнопок */
volatile uint8_t button1_pressed_flag = 0;
volatile uint8_t button2_press_event_flag = 0;
volatile uint8_t button3_press_event_flag = 0;

void SysTick_Handler(void){
    system_time_ms++;
}

/* EXTI0 -> PA0 */
void EXTI0_IRQHandler(void){
    if(EXTI->PR & EXTI_PR_PR0){
        button1_pressed_flag = 1;
        EXTI->PR = EXTI_PR_PR0;
    }
}

/* EXTI9_5 -> PA5 */
void EXTI9_5_IRQHandler(void){
    if(EXTI->PR & EXTI_PR_PR5){
        button2_press_event_flag = 1;
        EXTI->PR = EXTI_PR_PR5;
    }
}

/* EXTI -> PC13 */
void EXTI15_10_IRQHandler(void){
    if(EXTI->PR & EXTI_PR_PR13){
        button3_press_event_flag = 1;
        EXTI->PR = EXTI_PR_PR13;
    }
}
