#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

/* Переменные состояния кнопок */
extern volatile uint32_t system_time_ms;
extern volatile uint8_t button1_pressed_flag; /* PA0 */
extern volatile uint8_t button2_press_event_flag; /* PA5 */
extern volatile uint8_t button3_press_event_flag; /* PC13 */

/* Обработчики прерываний */
void SysTick_Handler(void);
void EXTI0_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

#endif /* INTERRUPT_H */
