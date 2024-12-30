#include "../../CMSIS/Devices/stm32f429xx.h"
#include "../../CMSIS/Devices/stm32f4xx.h"

#define RCC_AHB1ENR (*(uint32_t*)(0x40023800UL + 0x30UL))

#define RCC_GPIOB_EN 0x02UL
#define RCC_GPIOC_EN 0x04UL

#define GPIOB_MODER (*(uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_OTYPER (*(uint32_t*)(0x40020400UL + 0x04UL))
#define GPIOB_OSPEEDR (*(uint32_t*)(0x40020400UL + 0x08UL))
#define GPIOB_PUPDR (*(uint32_t*)(0x40020400UL + 0x0CUL))
#define GPIOB_BSRR (*(uint32_t*)(0x40020400UL + 0x18UL))
#define GPIOB_ODR (*(uint32_t*)(0x40020400UL + 0x14UL))

#define GPIOB_MODE_PIN0_OUT 0x0001UL
#define GPIOB_OTYPE_PIN0_PP 0x0000UL
#define GPIOB_OSPEED_PIN0_MID 0x0001UL
#define GPIOB_PUPDR_PIN0_NOPUPD 0x0000UL
#define GPIOB_BSRR_PIN0_SET 0x000001UL
#define GPIOB_BSRR_PIN0_RESET 0x010000UL

#define GPIOC_IDR (*(uint32_t*)(0x40020800UL + 0x10UL))
#define GPIOC_IDR_PIN0 0x000001UL
#define GPIOC_IDR_PIN13 0x002000UL

#define SYSTLOAD 179999

#define READ_BIT_Self(REG, BIT) (REG & BIT)
#define SET_BIT_Self(REG, BIT) (REG |= BIT)

void RCC_Init(void);
void GPIO_Init(void);
void ITR_Init(void);
void SysTick_Init(void);
