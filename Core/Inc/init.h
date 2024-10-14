#include <stdint.h>

#define RCC_GPIO_EN         *(uint32_t *)(0x40023800UL + 0x30UL)

#define RCC_GPIOBEN         0x02
#define RCC_GPIOCEN         0x04

#define GPIOB_MODER         *(uint32_t *)(0x40020400UL + 0x00UL)
#define GPIOB_OSPEEDR       *(uint32_t *)(0x40020400UL + 0x08UL)
#define GPIOB_PUPDR         *(uint32_t *)(0x40020400UL + 0x0CUL)
#define GPIOB_BSRR          *(uint32_t *)(0x40020400UL + 0x18UL)

#define GPIO_PIN_7_OUT      0x4000UL   
#define GPIO_PIN_7_MED      0X4000UL
#define GPIO_PIN_7_SET      0X80UL
#define GPIO_PIN_7_RESET    0X800000UL

#define GPIOC_IDR           *(uint32_t *)(0x40020800UL + 0x10UL)

#define GPIO_PIN_13         0X2000UL