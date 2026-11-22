#include <stdint.h>

#define RCC         0x40023800UL
#define AHB1ENR     0x30UL
#define GPIO_A      0x40020000UL
#define GPIO_B      0x40020400UL
#define GPIO_C      0x40020800UL
#define GPIO_E      0x40021000UL
#define GPIO_F      0x40021400UL
#define MODER       0x00UL
#define OTYPER      0x04UL
#define OSPEEDR     0x08UL
#define PUPDR       0x0CUL
#define IDR         0x10UL
#define BSRR        0x18UL

#define BR_MUL      0x00010000UL
#define BS_B0       0x0001UL
#define BS_B1       0x0002UL
#define BS_B2       0x0004UL
#define BS_B3       0x0008UL
#define BS_B4       0x0010UL
#define BS_B5       0x0020UL
#define BS_B6       0x0040UL
#define BS_B7       0x0080UL
#define BS_B8       0x0100UL
#define BS_B9       0x0200UL
#define BS_B10      0x0400UL
#define BS_B11      0x0800UL
#define BS_B12      0x1000UL
#define BS_B13      0x2000UL
#define BS_B14      0x4000UL
#define BS_B15      0x8000UL

#define RCC_GPIO_A_EN   0x01UL
#define RCC_GPIO_B_EN   0x02UL
#define RCC_GPIO_C_EN   0x04UL
#define RCC_GPIO_D_EN   0x08UL
#define RCC_GPIO_E_EN   0x10UL
#define RCC_GPIO_F_EN   0x20UL

#define IDR_B5         0x0020UL
#define IDR_B6         0x0040UL
#define IDR_B7         0x0080UL

#define BIT_SET(REG, VAL) (*(uint32_t*)(REG)|=(VAL))
#define BIT_READ(REG, VAL) (*(uint32_t*)(REG)&(VAL))

void GPIO_init(void);