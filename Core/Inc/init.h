#include <stdint.h>
#include <stdbool.h>
#include "C:\Users\parni\Git_repos\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\STM32F411xE\stm32f411xe.h"
#include "C:\Users\parni\Git_repos\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\stm32f4xx.h"

#define freq_0_4Hz     2500
#define freq_1_9Hz     526
#define freq_2_6Hz     385
#define freq_0_3Hz     3333
#define freq_1_6Hz     625
#define freq_2_3Hz     435



void RCC_Init(void);
void GPIO_Init(void);
void ITR_Init(void);
void SysTick_Init(void);
void SetLEDState(bool ledB1, bool ledB15, bool ledB14, bool ledB13, bool ledB10, bool ledB4);
void EXTI15_10_IRQHandler(void);
void first_led_mode(uint8_t freq_mode);
void second_led_mode(uint8_t freq_mode);