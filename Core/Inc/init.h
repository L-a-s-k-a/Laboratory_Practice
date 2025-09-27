#include <stdint.h>

#include "CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "CMSIS/Devices/STM32F4xx/Inc/STM32F429ZI/stm32f429xx.h"

#define GPIO1 *(uint32_t *)(0x40020400 + 0x02) //GPIOB 2
#define GPIO2 *(uint32_t *)(0x40020400 + 0x06) //GPIOB 6
#define GPIO3 *(uint32_t *)(0x40020C00 + 0xB) //GPIOD 11
#define GPIO4 *(uint32_t *)(0x40020C00 + 0xC) //GPIOD 12
#define GPIO5 *(uint32_t *)(0x40020C00 + 0xD) //GPIOD 13
#define GPIO6 *(uint32_t *)(0x40021000 + 0x02) //GPIOE 2
#define GPIO_BUT1 *(uint32_t *)(0x40018000 + 0x02) //GPIOG 2
#define GPIO_BUT2 *(uint32_t *)(0x40018000 + 0x03) //GPIOG 3


#define pin_2_in 0x00 //вход для кнопки 1
#define pin_3_in 0x00 //вход для кнопки 2
#define pin_2_out 0x20 //пин 2 на выход
#define pin_3_out 0x80 //пин 3 на выход 
#define pin_6_out 0x2000 //пин 6 на выход
#define pin_11_out 0x80000 //пин 11 выход
#define pin_12_out 0x200000 //пин 12 выход
#define pin_13_out 0x800000 // пин 13 выход
