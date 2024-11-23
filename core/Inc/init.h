#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"


#define RCC_AHB1ENR                *(uint32_t *)(0x40023800UL + 0x30UL)

#define RCC_AHB1ENR_GPIOBEN        0X02
#define RCC_AHB1ENR_GPIOCEN        0X02

#define GPIOB_MODER                *(uint32_t *)(0x40020400UL + 0x00UL)
#define GPIOB_OSPEEDR              *(uint32_t *)(0x40020400UL + 0x08UL)
#define GPIOB_BSRR                 *(uint32_t *)(0x40020400UL + 0x18UL)

#define GPIO_PIN_7_OUT            0x4000UL  
#define GPIO_PIN_7_MED            0x4000UL 
#define GPIO_PIN_7_SET            0x80UL 
#define GPIO_PIN_7_RESET          0x8000000UL 

#define GPIOC_IDR                  *(uint32_t *)(0x40020800UL + 0x10UL)

#define GPIO_PIN_13               0x2000UL 

#define READ_BIT_SELF(REG,BIT) ((REG) & (BIT))
#define SET_BIT_SELF(REG,BIT) ((REG) |= (BIT))

void GPIO_Ini_Self_Def(void);
void GPIO_Ini_Self_CMSIS(void);