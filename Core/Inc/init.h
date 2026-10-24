#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32f446xx.h>

// Тактирование
#define RCC_AHB1ENR         *(uint32_t*)(0x40023800UL + 0x30UL)
#define RCC_GPIOA_EN        0x01UL
#define RCC_GPIOB_EN        0x02UL
#define RCC_GPIOC_EN        0x04UL

// Порт A светодиоды PA5, PA6, PA7
#define GPIOA_MODER         *(uint32_t*)(0x40020000UL + 0x00UL)
#define GPIOA_BSRR          *(uint32_t*)(0x40020000UL + 0x18UL)
#define GPIO_OUTPUT_5       0x400UL      // MODER5 = 01
#define GPIO_OUTPUT_6       0x1000UL     // MODER6 = 01
#define GPIO_OUTPUT_7       0x4000UL     // MODER7 = 01
#define GPIO_BS_5           0x20UL
#define GPIO_BR_5           0x200000UL
#define GPIO_BS_6           0x40UL
#define GPIO_BR_6           0x400000UL

// Порт B кнопка PB9
#define GPIOB_MODER         *(uint32_t*)(0x40020400UL + 0x00UL)
#define GPIOB_PUPDR         *(uint32_t*)(0x40020400UL + 0x0CUL)
#define GPIOB_IDR           *(uint32_t*)(0x40020400UL + 0x10UL)
#define GPIO_OUTPUT_8       0x10000UL    // MODER8 = 01
#define GPIO_PULLUP_9       0x40000UL    // PUPDR9 = 01 подтяжка к питанию
#define GPIO_ID_9           0x200UL

// Порт C кнопка и светодиод PC7
#define GPIOC_MODER         *(uint32_t*)(0x40020800UL + 0x00UL)
#define GPIOC_PUPDR         *(uint32_t*)(0x40020800UL + 0x0CUL)
#define GPIOC_IDR           *(uint32_t*)(0x40020800UL + 0x10UL)
#define GPIOC_BSRR          *(uint32_t*)(0x40020000UL + 0x18UL)
#define GPIO_PULLUP_7       0x4000UL     // PUPDR7 = 01
#define GPIO_ID_7           0x80UL
#define GPIO_BS_7           0x80UL
#define GPIO_BR_7           0x800000UL

#define BIT_READ(REG, VAL)  ((REG) & (VAL))
#define BIT_SET(REG, VAL)   ((REG) |= (VAL))

void dop_zadanie(void);
void delay(volatile uint32_t time);
void __init_mem(void);
void __init_myself_def(void);
void __init_CMSIS(void);
