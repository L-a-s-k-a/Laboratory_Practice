#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"
void GPIO_Init();
void GPIO_Init_const_LED_button();
void LED1_out(); //LED1 - PA3
void LED2_out(); //LED2 - PC0
void LED3_out(); //LED3 - PC3
void LED1_in();
void LED2_in();
void LED3_in();