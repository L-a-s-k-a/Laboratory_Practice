
#ifndef INC_INIT_H_
#define INC_INIT_H_
#include "main.h"
#include "string.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f411xe.h"

#define Led_Red_ON GPIOB->BSRR = GPIO_BSRR_BR_3;
#define Led_Red_OFF GPIOB->BSRR = GPIO_BSRR_BS_3;

#define Led_Yellow_ON GPIOB->BSRR = GPIO_BSRR_BR_5;
#define Led_Yellow_OFF GPIOB->BSRR = GPIO_BSRR_BS_5;

#define Led_Green_ON GPIOB->BSRR = GPIO_BSRR_BR_4;
#define Led_Green_OFF GPIOB->BSRR = GPIO_BSRR_BS_4;

#define all_leds_off GPIOB->BSRR = GPIO_BSRR_BS_3 | GPIO_BSRR_BS_4| GPIO_BSRR_BS_5;

void RCC_init(void);
void GPIO_init(void);
#endif /* INC_INIT_H_ */
