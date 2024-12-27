
#include<stdint.h>
#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"
void RCC_Ini(void);
void GPIO_Ini(void);
void EXTI_ITR_Ini(void);
void SysTick_Init(void);
void update_led_state(void);
#define LED1_PIN    GPIO_BSRR_BS14
#define LED2_PIN    GPIO_BSRR_BS7
#define LED3_PIN    GPIO_BSRR_BS0
#define LED4_PIN    GPIO_BSRR_BS2
#define LED5_PIN    GPIO_BSRR_BS6
#define LED6_PIN    GPIO_BSRR_BS1