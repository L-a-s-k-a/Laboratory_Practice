#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"
#include <stdint.h>


#define GPIOC_MODER (*(uint32_t*)(0x40020800UL + 0x00UL))
#define GPIOC_OTYPER (*(uint32_t*)(0x40020800UL + 0x04UL))
#define GPIOC_OSPEEDR (*(uint32_t*)(0x40020800UL + 0x08UL))
#define GPIOC_PUPDR (*(uint32_t*)(0x40020800UL + 0x0CUL))
#define GPIOC_BSRR (*(uint32_t*)(0x40020800UL + 0x18UL))
#define GPIOC_MODE_OUT_PIN6 0x001000UL
#define GPIOC_TYPE_PP_PIN6 0x000000UL
#define GPIOC_SPEED_M_PIN6 0x00400UL
#define GPIOC_PUPDR_NPUPD_PIN6 0x000000UL
#define GPIOC_PIN6_S 0x000040UL
#define GPIOC_PIN6_R 0x800000UL


void RCC_Init(void);
void ITR_init(void);
void SysTick_Init(void);
void GPIO_Ini(void);
