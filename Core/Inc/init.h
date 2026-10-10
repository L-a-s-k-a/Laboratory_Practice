#ifndef INIT_H
#define INIT_H

#include <stdint.h>

// Регистры RCC
#define RCC_GPIO_EN           (*(volatile uint32_t*)(0x40023800UL + 0x30UL))
#define RCC_GPIOB_EN          0x02UL
#define RCC_GPIOC_EN          0x04UL

// Регистры GPIOB
#define GPIOB_MODER           (*(volatile uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_OTYPER          (*(volatile uint32_t*)(0x40020400UL + 0x04UL))
#define GPIOB_OSPEEDR         (*(volatile uint32_t*)(0x40020400UL + 0x08UL))
#define GPIOB_PUPDR           (*(volatile uint32_t*)(0x40020400UL + 0x0CUL))
#define GPIOB_BSRR            (*(volatile uint32_t*)(0x40020400UL + 0x18UL))

// Регистры GPIOC
#define GPIOC_MODER           (*(volatile uint32_t*)(0x40020800UL + 0x00UL))
#define GPIOC_PUPDR           (*(volatile uint32_t*)(0x40020800UL + 0x0CUL))
#define GPIOC_IDR             (*(volatile uint32_t*)(0x40020800UL + 0x10UL))
#define GPIOC_BSRR            (*(volatile uint32_t*)(0x40020800UL + 0x18UL))

// Маски для BSRR портов светодиодов (GPIOB)
#define GPIOB_BSRR_PIN0_SET   0x00000001UL
#define GPIOB_BSRR_PIN0_RESET 0x00010000UL

#define GPIOB_BSRR_PIN7_SET   0x00000080UL
#define GPIOB_BSRR_PIN7_RESET 0x00800000UL

#define GPIOB_BSRR_PIN14_SET  0x00004000UL
#define GPIOB_BSRR_PIN14_RESET 0x40000000UL

// Маски для BSRR управляемого порта (GPIOC)
#define GPIOC_BSRR_PIN3_SET   0x00000008UL
#define GPIOC_BSRR_PIN3_RESET 0x00080000UL

// Маски для чтения IDR (GPIOC)
#define GPIOC_IDR_PIN0        0x0001UL
#define GPIOC_IDR_PIN2        0x0004UL
#define GPIOC_IDR_PIN3        0x0008UL

// Функциональные макросы
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))  
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))   

#endif
