#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"

void RCC_Ini(void);
void GPIO_Ini(void);
void EXTI_ITR_Ini(void);
void SysTick_Init(void);

void update_led_pair(uint8_t stage);
void reverse_led_pair(uint8_t stage);
void toggle_led(uint8_t led);

#define BUTTON1_PIN GPIO_IDR_ID8
#define BUTTON2_PIN GPIO_IDR_ID9
#define BUTTON3_PIN GPIO_IDR_ID10

#define LED1_PIN GPIO_ODR_OD14
#define LED2_PIN GPIO_ODR_OD7
#define LED3_PIN GPIO_ODR_OD0
#define LED4_PIN GPIO_ODR_OD15
#define LED5_PIN GPIO_ODR_OD13
#define LED6_PIN GPIO_ODR_OD12

#endif

