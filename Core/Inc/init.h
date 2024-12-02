#include "stm32f4xx.h"
#include "stm32f429xx.h"
#include <stdint.h>

void GPIO_Ini(void);
void On_Led(int NumLed, int NumBut);
void Ch_Led_On(int NumS);
void Ch_Led_Off();
void GPIOA_button_OUT();

#define GPIOB_MODER (*(uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_OTYPER (*(uint32_t*)(0x40020400UL + 0x04UL)) 
#define GPIOB_OSPEEDR (*(uint32_t*)(0x40020400UL + 0x08UL)) 
#define GPIOB_PUPDR (*(uint32_t*)(0x40020400UL + 0x0CUL))
#define GPIOB_BSRR (*(uint32_t*)(0x40020400UL + 0x18UL))

#define GPIOB_MODE_PIN0_OUT 0x0001UL 
#define GPIOB_OTYPE_PIN0_PP 0x0000UL 
#define GPIOB_OSPEED_PIN0_MID 0x0001UL 
#define GPIOB_PUPDR_PIN0_NOPUPD 0x0000UL
#define GPIOB_BSRR_PIN0_SET 0x000001UL 
#define GPIOB_BSRR_PIN0_RESET 0x010000UL

#define READ_BIT_Self(REG, BIT) (REG & BIT)
#define SET_BIT_Self(REG, BIT) (REG |= BIT)