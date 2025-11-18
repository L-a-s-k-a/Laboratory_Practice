#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

extern volatile uint8_t led1_state;
extern volatile uint8_t led2_state;
extern volatile uint8_t led3_state;
extern volatile uint8_t led4_state;
extern volatile uint8_t led5_state;
extern volatile uint8_t led6_state;

void ITR_Init(void);                  // Прерывание на PC13
void SysTick_Init_1ms(void);          // Настройка SysTick на 1мс
void Clock_Init_HSE_PLL_168MHz(void); // Настройка тактирования HSE -> PLL 168MHz

void Buttons_GPIO_Init(void);         // Инициализация кнопок
void Buttons_EXTI_Init(void);

void LEDs_GPIO_Init(void);
void MCO_init(void);

void LED_AllOff(void);
void LED_On_Index(uint8_t index);
void LED_Off_Index(uint8_t index);
void LED_SetActive(uint8_t index);

#endif /* INIT_H */
