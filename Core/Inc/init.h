#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"

#define RCC_GPIO_EN             (*(uint32_t*)(0x40023800UL + 0x30UL))
#define RCC_GPIOB_EN            0x02UL
#define RCC_GPIOC_EN            0x04UL

#define GPIOB_MODER             (*(uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_BSRR              (*(uint32_t*)(0x40020400UL + 0x18UL))
#define GPIOC_IDR               (*(uint32_t*)(0x40020800UL + 0x10UL))

#define GPIOB_MODE_PIN0_OUT     0x00000001UL // PB0
#define GPIOB_MODE_PIN7_OUT     0x00000040UL // PB7
#define GPIOB_MODE_PIN14_OUT    0x00004000UL // PB14
#define GPIOB_MODE_PIN15_OUT    0x00008000UL // PB15 (外接LED)

#define READ_GPIO_C13           (GPIOC_IDR & 0x2000UL) // 读 GPIOC 的引脚 13
#define SET_GPIO_B(pin)         (GPIOB_BSRR = (1 << (pin))) // 设置 GPIOB 的引脚
#define RESET_GPIO_B(pin)       (GPIOB_BSRR = (1 << ((pin) + 16))) // 重置 GPIOB 的引脚

void GPIO_Ini(void);