#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F411xE/stm32f411xe.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include <stdbool.h>

void GPIO_INIT(void);
void RCC_INIT(void);
void IQR_INIT(void);
void SYSTIMER_INIT(void);
void working_mode(uint8_t num, uint8_t LED_OPTION[2][6]);
void Conf_Diodes(uint8_t LED_OPTIONS);