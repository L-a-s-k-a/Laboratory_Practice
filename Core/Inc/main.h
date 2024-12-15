#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

// RCC 时钟使能
#define RCC_AHB1ENR       (*(uint32_t *)(0x40023800UL + 0x30UL))
#define RCC_GPIOA_EN      (0x01UL)  // GPIOA 时钟
#define RCC_GPIOB_EN      (0x02UL)  // GPIOB 时钟
#define RCC_GPIOC_EN      (0x04UL)  // GPIOC 时钟
#define RCC_GPIOF_EN      (0x20UL)  // GPIOF 时钟

// GPIO 寄存器定义
#define GPIOA_MODER       (*(uint32_t *)(0x40020000UL + 0x00UL))
#define GPIOA_BSRR        (*(uint32_t *)(0x40020000UL + 0x18UL))

#define GPIOB_MODER       (*(uint32_t *)(0x40020400UL + 0x00UL))
#define GPIOB_BSRR        (*(uint32_t *)(0x40020400UL + 0x18UL))

#define GPIOC_MODER       (*(uint32_t *)(0x40020800UL + 0x00UL))
#define GPIOC_BSRR        (*(uint32_t *)(0x40020800UL + 0x18UL))
#define GPIOC_IDR         (*(uint32_t *)(0x40020800UL + 0x10UL))

#define GPIOF_MODER       (*(uint32_t *)(0x40021400UL + 0x00UL))
#define GPIOF_PUPDR       (*(uint32_t *)(0x40021400UL + 0x0CUL))
#define GPIOF_IDR         (*(uint32_t *)(0x40021400UL + 0x10UL))

// 板载 LED 宏定义
#define LED1_ON           (GPIOB_BSRR = (1 << 0))       // PB0
#define LED1_OFF          (GPIOB_BSRR = (1 << (0 + 16)))
#define LED2_ON           (GPIOB_BSRR = (1 << 7))       // PB7
#define LED2_OFF          (GPIOB_BSRR = (1 << (7 + 16)))
#define LED3_ON           (GPIOB_BSRR = (1 << 14))      // PB14
#define LED3_OFF          (GPIOB_BSRR = (1 << (14 + 16)))

// 外接 LED 宏定义
#define EXT_LED1_ON       (GPIOA_BSRR = (1 << 3))       // PA3
#define EXT_LED1_OFF      (GPIOA_BSRR = (1 << (3 + 16)))
#define EXT_LED2_ON       (GPIOC_BSRR = (1 << 0))       // PC0
#define EXT_LED2_OFF      (GPIOC_BSRR = (1 << (0 + 16)))
#define EXT_LED3_ON       (GPIOC_BSRR = (1 << 3))       // PC3
#define EXT_LED3_OFF      (GPIOC_BSRR = (1 << (3 + 16)))

// 板载和外接按钮宏定义
#define BUTTON1_PRESSED   (!(GPIOC_IDR & (1 << 13)))    // PC13 板载按钮
#define BUTTON2_PRESSED   (!(GPIOF_IDR & (1 << 3)))     // PF3 外接按钮 1
#define BUTTON3_PRESSED   (!(GPIOF_IDR & (1 << 5)))     // PF5 外接按钮 2

void GPIO_Init(void);
void Delay(volatile uint32_t time);

#endif
