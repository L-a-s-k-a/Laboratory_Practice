#include "../../CMSIS/Devices/stm32f429xx.h"
#include "../../CMSIS/Devices/stm32f4xx.h"

#define RCC_GPIO_EN (*(uint32_t *)(0x40023800UL + 0x30UL))
#define RCC_GPIOB_EN 0x02UL

#define GPIOB_MODER (*(uint32_t *)(0x40020400UL + 0x00UL))
#define GPIOB_IDR (*(uint32_t *)(0x40020400UL + 0x10UL))

#define GPIOB_MODE_PIN8_OUT (1 << 16)
#define GPIOB_MODE_PIN9_OUT (1 << 18)
#define GPIOB_MODE_PIN10_OUT (1 << 20)

#define GPIOB_IDR_PIN11 (1 << 11) // Port B Read bit 3
#define GPIOB_IDR_PIN12 (1 << 12)

#define GPIOB_BSRR_PIN8_SET (1 << 8)
#define GPIOB_BSRR_PIN8_RESET (1 << (8 + 16))
#define GPIOB_BSRR_PIN9_SET (1 << 9)
#define GPIOB_BSRR_PIN9_RESET (1 << (9 + 16))
#define GPIOB_BSRR_PIN10_SET (1 << 10)
#define GPIOB_BSRR_PIN10_RESET (1 << (10 + 16))

#define GPIOB_BSRR (*(uint32_t *)(0x40020400UL + 0x18UL)) // Port bit set/reset register

#define BIT_SET(REG, BIT) (REG |= BIT) // Set bit function
#define BIT_READ(REG, BIT) (REG & BIT) // Read bit function
#define BIT_RESET(REG, BIT)  REG &= ~BIT

#define GPIOB_ADDRESS       0x40020400UL
#define GPIOB_CUSTOM        ((GPIO_TYPEDEF *) GPIOB_ADDRESS)
#define GPIO_MODERX_0(X)    (0x1UL << (X * 2))
#define GPIO_MODERX_1(X)    (0x2UL << (X * 2))
#define GPIO_OSPEEDRX_0(X)  (0x1UL << (X * 2))
#define GPIO_OSPEEDRX_1(X)  (0x2UL << (X * 2))

typedef struct {
    uint32_t MODER;   // 0x00
    uint32_t OTYPER;  // 0x04
    uint32_t OSPEEDR; // 0x08
    uint32_t PUPDR;   // 0x0C
    uint32_t IDR;     // 0x10
    uint32_t ODR;     // 0x14
    uint32_t BSRR;    // 0x18
    uint32_t LCKR;    // 0x1C
    uint32_t AFRL;    // 0x20
    uint32_t AFRH;    // 0x24
} GPIO_TYPEDEF;

void GPIO_Init();
