#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F411xE/stm32f411xe.h"
#include <stdbool.h>

#define Frequency_First_Mode_0_4Hz      2500
#define Frequency_First_Mode_1_9Hz      526
#define Frequency_First_Mode_2_6Hz      385
#define Frequency_Second_Mode_0_3Hz     3333
#define Frequency_Second_Mode_1_6Hz     625
#define Frequency_Second_Mode_2_3Hz     435

void GPIO_Init(void);
void RCC_Init(void);
void IQR_Init(void);
void SysTick_Init(void);
void Mode_Flicker_First(uint8_t Frequency);
void Mode_Flicker_Second(uint8_t Frequency);