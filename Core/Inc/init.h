#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F411xE/stm32f411xe.h"
void RCC_Ini(void);
void GPIO_Ini(void);
void EXTI_ITR_Ini(void);
void SysTick_Init(void);
void WorkHelpOn(uint16_t ValLed);
void WorkHelpOff(uint16_t ValLed);
void LedPush(uint16_t led[6][2]);
void LedMerz(uint16_t led[6][2]);
void LedOn(uint16_t led[6][2]);
void Work(uint16_t led[6][2]);
void Clear(void);


