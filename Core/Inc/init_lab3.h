#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"

void GPIO_Init_Led_and_button(void);                                  // Прототип функции включения и настройки шин тактирования (кнопки и светодиоды)
void RCC_init_clocking(void);                                         // Прототип функции настройки тактирования микроконтроллера
void ITR_init(void);                                                  // Прототип функции настройки прерываний по кнопке
void TIM1_PWM_PE14(void);                                             // Прототип функции настройки ШИМ-сигнала на порт PE14
void TIM3_Init(void);                                                 // Прототип функции настройки обычного таймера
