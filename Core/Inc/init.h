#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h"

#define GPIO_B_MODER  (*(uint32_t*)(0x40020400UL + 0x00UL))
#define GPIO_B_OTYPER (*(uint32_t*)(0x40020400UL + 0x04UL))
#define GPIO_B_OSPEEDR (*(uint32_t*)(0x40020400UL + 0x08UL))
#define GPIO_B_PUPDR (*(uint32_t*)(0x40020400UL + 0x0CUL))
#define GPIO_B_BSRR (*(uint32_t*)(0x40020400UL + 0x18UL))

#define GPIO_B_MODE_PB0_OUT 0x01UL
#define GPIO_B_OTYPE_PB0_PP 0x0000UL
#define GPIO_B_OSPEED_PB0_MID 0x01UL
#define GPIO_B_PUPDR_PB0_NOPUPD 0x0000UL
#define GPIO_B_BSRR_PB0_SET 0x01UL
#define GPIO_B_BSRR_PB0_RESET 0x10000UL

#define GPIO_E_MODER  (*(uint32_t*)(0x40021000UL + 0x00UL))
#define GPIO_E_OTYPER (*(uint32_t*)(0x40021000UL + 0x04UL))
#define GPIO_E_OSPEEDR (*(uint32_t*)(0x40021000UL + 0x08UL))
#define GPIO_E_PUPDR (*(uint32_t*)(0x40021000UL + 0x0CUL))
#define GPIO_E_BSRR (*(uint32_t*)(0x40021000UL + 0x18UL))

#define GPIO_E_MODE_PE0_OUT 0x01UL
#define GPIO_E_OTYPE_PE0_PP 0x0000UL
#define GPIO_E_OSPEED_PE0_MID 0x01UL
#define GPIO_E_PUPDR_PE0_NOPUPD 0x0000UL
#define GPIO_E_BSRR_PE0_SET 0x01UL
#define GPIO_E_BSRR_PE0_RESET 0x10000UL

#define S_BIT(R, B) (R |= B)

#define CL_DEL_E 0x00

void Register_Init(void);
void Led_Init(void);
void All_Ports_Input(void);
void All_Ports_Output(void);
void First_Port_Output(void);
void Second_Port_Output(void);
void Third_Port_Output(void);
void delay(void);




