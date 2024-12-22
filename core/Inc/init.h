#include<stdint.h>
#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"
void RCC_Ini(void);
void GPIO_Ini(void);
void EXTI_ITR_Ini(void);
void SysTick_Init(void);
void update_led_state(void);



#define LED1_PIN    GPIO_BSRR_BS14  // BSRR2
#define LED2_PIN    GPIO_BSRR_BS7  // BSRR6
#define LED3_PIN    GPIO_BSRR_BS0  // BSRR1
#define LED4_PIN    GPIO_BSRR_BS2  // BSRR4
#define LED5_PIN    GPIO_BSRR_BS6  // BSRR3
#define LED6_PIN    GPIO_BSRR_BS1  // BSRR5