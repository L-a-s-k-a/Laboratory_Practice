#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"

#define RCC_GPIO_EN                 (*(uint32_t*)(0x40023800UL + 0x30UL)) //адрес RCC + регистра
#define RCC_GPIOB_EN                0x02UL
#define RCC_GPIOC_EN                0x04UL
#define GPIOB_MODER                 (*(uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_OTYPER                (*(uint32_t*)(0x40020400UL + 0x04UL))
#define GPIOB_OSPEEDR               (*(uint32_t*)(0x40020400UL + 0x08UL))
#define GPIOB_PUPDR                 (*(uint32_t*)(0x40020400UL + 0x0CUL))
#define GPIOB_BSRR                  (*(uint32_t*)(0x40020400UL + 0x18UL))
#define GPIOC_IDR                   (*(uint32_t*)(0x40020800UL + 0x10UL))
#define GPIOC_IDR_PIN13             0x0002000UL
#define GPIOB_MODE_PIN0_OUT         0x0000001UL
#define GPIOB_MODE_PIN7_OUT         0x0000040UL
#define GPIOB_MODE_PIN14_OUT        (0x1UL << 14*2)
#define GPIOB_OTYPE_PIN0_PP         0x0000000UL
#define GPIOB_OTYPE_PIN7_PP         0x0040000UL
#define GPIOB_OTYPE_PIN14_PP        0x4000000UL
#define GPIOB_OSPEED_PIN0_MID       0x0000001UL
#define GPIOB_OSPEED_PIN7_MID       0x0000040UL
#define GPIOB_OSPEED_PIN14_MID      *((uint32_t *)(0x40020400UL + 0x00UL))
#define GPIOB_PUPDR_PIN0_NOPUPD     0x0000000UL
#define GPIOB_PUPDR_PIN7_NOPUPD     0x0000000UL
#define GPIOB_PUPDR_PIN14_NOPUPD    0x0000000UL
#define GPIOB_BSRR_PIN0_SET         0x0000001UL
#define GPIOB_BSRR_PIN7_SET         0x0000080UL
#define GPIOB_BSRR_PIN14_SET        0x0004000UL
#define GPIOB_BSRR_PIN0_RESET       0x0010000UL
#define SELF_READ_BIT(REG, BIT)     (REG & BIT)
#define SELF_SET_BIT(REG, BIT)      (REG |= BIT)

//void GPIO_Init_CMSIS(void);
void GPIO_Init_Self_Def(void);
//void GPIO_Init_Mem(void);