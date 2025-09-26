#include <stdint.h>

#include "CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "CMSIS/Devices/STM32F4xx/Inc/STM32F429ZI/stm32f429xx.h"

// *(uint32_t *)(0x40020C00 + 0xB) |= 0x40000 GPIOD 11
// *(uint32_t *)(0x40020C00 + 0xC) |= 0x200000 GPIOD 12
// *(uint32_t *)(0x40020C00 + 0xD) |=  0x800000 GPIOD 13
// *(uint32_t *)(0x40020400 + 0x02) |= 0x20 //GPIOB 2
// *(uint32_t *)(0x40020400 + 0x06) |= 0x2000  /GPIOB 6
// *(uint32_t *)(0x40021000 + 0x02) |= 0x20 //GPIOE 2
// *(uint32_t *)(0x40018000 + 0x02) |= 0x20 //GPIOG 2
// *(uint32_t *)(0x40018000 + 0x03) |= 0x80 //GPIOG 3

#define GPIOB *(uint32_t *)(0x40020400)
#define GPIOD *(uint32_t *)(0x40020C00)
#define GPIOE *(uint32_t *)(0x40021000)
#define GPIOG *(uint32_t *)(0x40018000)

#define pin_2 *(uint32_t *)(0x02)
#define pin_3 *(uint32_t *)(0x03)
#define pin_6 *(uint32_t *)(0x02)
#define pin_11 *(uint32_t *)(0xB)
#define pin_12 *(uint32_t *)(0xC)
#define pin_13 *(uint32_t *)(0xD)
