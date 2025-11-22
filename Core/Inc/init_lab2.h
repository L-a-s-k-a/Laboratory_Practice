#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include <stdbool.h>

#define LED1_ON         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);          // Макрос включения светодиода №1
#define LED2_ON         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);          // Макрос включения светодиода №2
#define LED3_ON         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);         // Макрос включения светодиода №3
#define LED4_ON         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8);          // Макрос включения светодиода №4
#define LED5_ON         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS9);          // Макрос включения светодиода №5
#define LED6_ON         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15);         // Макрос включения светодиода №6

#define LED1_OFF        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);          // Макрос выключения светодиода №1
#define LED2_OFF        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);          // Макрос выключения светодиода №2
#define LED3_OFF        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);         // Макрос выключения светодиода №3
#define LED4_OFF        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);          // Макрос выключения светодиода №4
#define LED5_OFF        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR9);          // Макрос выключения светодиода №5
#define LED6_OFF        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);         // Макрос выключения светодиода №6

void GPIO_Init_Led_and_button(void);                                  // Прототип функции включения и настройки шин тактирования (кнопки и светодиоды)
void RCC_init_clocking(void);                                         // Прототип функции настройки тактирования микроконтроллера
void ITR_init(void);                                                  // Прототип функции настройки прерываний по кнопке
void SysTick_Init(void);                                              // Прототип функции настройки системного счетчика 