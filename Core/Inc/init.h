#include <stdint.h>

#define RCC_GPIO_EN (*(uint32_t*)(0x40023800UL + 0x30UL))
 
#define GPIOB_MODER (*(uint32_t*)(0x40020400UL + 0x00UL)) 

#define GPIOB_OTYPER (*(uint32_t*)(0x40020400UL + 0x04UL)) 
#define GPIOB_OSPEEDR (*(uint32_t*)(0x40020400UL + 0x08UL)) 
#define GPIOB_PUPDR (*(uint32_t*)(0x40020400UL + 0x0CUL))

#define RCC_GPIOB_EN 0x02UL 
#define RCC_GPIOC_EN 0x04UL 
#define GPIOB_MODE_PIN7_OUT 0x4000UL 
#define GPIOB_OTYPE_PIN7_PP 0x0000UL 
#define GPIOB_OSPEED_PIN7_MID 0x4000UL 
#define GPIOB_PUPDR_PIN7_NOPUPD 0x0000UL

#define READ_GPIO_C13 (*(uint32_t*)(0x40020800UL + 0x10UL) & 0x2000UL) 
#define SET_GPIO_B7 (*(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80) 
#define RESET_GPIO_B7 (*(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000)

void GPIO_Ini(void);