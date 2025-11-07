#include <stdint.h>

// Адреса регистров
// #define GPIOA_IDR  (*(uint32_t*)(0x40010808))
// #define GPIOA_BSRR (*(uint32_t*)(0x40010810))
// #define GPIOA_BRR  (*(uint32_t*)(0x40010814))

#define CRL1 (0x00)
#define CRH1 (0x04)
#define IDR11 (0x08)
#define ODR11 (0x0C)
#define BSRR1 (0x10)
#define BRR1 (0x14)

#define RCC_APB2ENR (*(uint32_t*)(0x40021000 + 0x18))
#define IOPAEN ((1UL << 2UL))

#define GPIO_PORT_A_CRL (*(uint32_t*)(0x40010800 + CRL1))
#define GPIO_PORT_A_CRH1 (*(uint32_t*)(0x40010800 + CRH1))
#define GPIO_PORT_A_IDR (*(uint32_t*)(0x40010800 + IDR11))
#define GPIO_PORT_A_ODR (*(uint32_t*)(0x40010800 + ODR11))
#define GPIO_PORT_A_BSRR (*(uint32_t*)(0x40010800 + BSRR1))
#define GPIO_PORT_A_BRR (*(uint32_t*)(0x40010800 + BRR1))

#define CRL_MODE_5 ((0x3UL << (20UL)))
#define CRL_MODE_6 ((0x3UL << (24UL)))
#define CRL_MODE_7 ((0x3UL << (28UL)))

#define CRL_CNF_5 ((0x3UL << (22UL)))
#define CRL_CNF_6 ((0x3UL << (26UL)))
#define CRL_CNF_7 ((0x3UL << (30UL)))

#define CRL_MODE5_2 ((0x2UL << (20UL)))
#define CRL_MODE6_2 ((0x2UL << (24UL)))
#define CRL_MODE7_2 ((0x2UL << (28UL)))

#define CRH_MODE_8 ((0x3UL << (0UL)))
#define CRH_MODE_9 ((0x3UL << (4UL)))
#define CRH_MODE_10 ((0x3UL << (8UL)))

#define CRH_CNF_8 ((0x3UL << (2UL)))
#define CRH_CNF_9 ((0x3UL << (6UL)))
#define CRH_CNF_10 ((0x3UL << (10UL)))

#define CRH_MODE8_2 ((0x2UL << (0UL)))
#define CRH_MODE9_2 ((0x2UL << (4UL)))
#define CRH_MODE10_2 ((0x2UL << (8UL)))

#define CRL_MODE0 ((0x3UL << (0UL)))
#define CRL_MODE1 ((0x3UL << (4UL)))

#define CRL_CNF0 ((0x3UL << (2UL)))
#define CRL_CNF1 ((0x3UL << (6UL)))

#define CRL_CNF0_1 ((0x2UL << (2UL)))
#define CRL_CNF1_1 ((0x2UL << (6UL)))

#define ODR0 ((0x1UL << 0UL))
#define ODR1 ((0x1UL << 1UL))

#define IDR0 ((0x1UL << 0UL))
#define IDR1 ((0x1UL << 1UL))

void GPIO_Ini(void);