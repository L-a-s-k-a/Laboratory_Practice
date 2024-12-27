#ifndef INIT_H
#define INIT_H

#include <stdint.h>

// RCC and GPIO definitions
#define RCC_GPIO_EN                 (*(uint32_t*)(0x40023800UL + 0x30UL)) 
#define RCC_GPIOC_EN                0x04UL 

// GPIOC configurations
#define GPIOC_MODER                 (*(uint32_t*)(0x40020800UL + 0x00UL))
#define GPIOC_OTYPER                (*(uint32_t*)(0x40020800UL + 0x04UL))
#define GPIOC_OSPEEDR               (*(uint32_t*)(0x40020800UL + 0x08UL))
#define GPIOC_PUPDR                 (*(uint32_t*)(0x40020800UL + 0x0CUL))
#define GPIOC_IDR                   (*(uint32_t*)(0x40020800UL + 0x10UL))
#define GPIOC_BSRR                  (*(uint32_t*)(0x40020800UL + 0x18UL))

// Buttons and LEDs
#define GPIOC_IDR_PIN8              0x0100UL // 按钮1 (PC8)
#define GPIOC_IDR_PIN9              0x0200UL // 按钮2 (PC9)
#define GPIOC_BSRR_LED1_ON          0x00000100UL
#define GPIOC_BSRR_LED1_OFF         0x01000000UL
#define GPIOC_BSRR_LED2_ON          0x00000200UL
#define GPIOC_BSRR_LED2_OFF         0x02000000UL
#define GPIOC_BSRR_LED3_ON          0x00000400UL
#define GPIOC_BSRR_LED3_OFF         0x04000000UL
#define GPIOC_BSRR_LED4_ON          0x00000800UL
#define GPIOC_BSRR_LED4_OFF         0x08000000UL

// Function prototypes
void GPIO_Init(void);
void delay(uint32_t s);

#endif // INIT_H
