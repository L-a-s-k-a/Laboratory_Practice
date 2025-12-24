#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"

extern volatile uint8_t counter1;

void Set_All_LEDs_Brightness(void);
void task1(void);
void  Switch_PWM(void);