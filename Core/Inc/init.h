// #include <stdint.h>
#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"

#define  RCC_AAB1ENR *(uint32_t *)(0x40023800UL + 0x30UL)

#define  RCC_GPIOBEN 0x02
#define  RCC_GPIOCEN 0x04

#define  GPIO_MODER    *(uint32_t *)(0x40020400UL + 0x00UL)
#define  GPIO_OSPEEDR  *(uint32_t *)(0x40020400UL + 0x08UL)
#define  GPIO_BSRR     *(uint32_t *)(0x40020400UL + 0x18UL)

#define  GPIO_PIN_7_OUT 0x4000UL
#define  GPIO_PIN_7_MED 0x4000UL
#define  GPIO_PIN_7_SET 0x80UL
#define  GPIO_PIN_7_RESET 0x800000UL

#define  GPIO_IDR *(uint32_t *)(0x40020800UL + 0x10UL)

#define  GPIO_PIN_13 0x2000UL

#define  READ_BIT_SELF(REG,BIT) ((REG)&(BIT))
#define  SET_BIT_SELF(REG,BIT) ((REG)|=(BIT))

void GPIO_Ini_Self_Def();
void GPIO_Ini_CMSIS();