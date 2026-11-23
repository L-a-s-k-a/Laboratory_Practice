#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32f446xx.h>

#define delay 200   // Время антидребезга, мс

extern volatile uint32_t global_tick_count;  // Счётчик миллисекунд

// Флаги нажатия кнопок (выставляются в обработчике прерывания, сбрасываются в main)
extern volatile uint8_t btn_mode;       // PB8 - смена режима
extern volatile uint8_t btn_freq;       // PB9 - смена частоты
extern volatile uint8_t btn_select;     // PB6 - выбор светодиода

void __RCC_Config(void);
void __GPIO_Config(void);
void __ITR_Config(void);
void SysTick_Init_Config(void);

void SysTick_Handler(void);
void EXTI9_5_IRQHandler(void);

#endif