#include <stdint.h>
#include "Core/Inc/init.h"

int main(void)
{
    *(uint32_t *)(0x40023800 + 0x30) |= 0x02;  //ТАКТИРОВАНИЕ

    GPIO1 |= pin_2_out;
    GPIO2 |= pin_6_out;
    GPIO3 |= pin_11_out;
    GPIO4 |= pin_12_out;
    GPIO5 |= pin_13_out;
    GPIO6 |= pin_2_out;
    GPIO_BUT1 |= pin_2_in; // наверно ин
    GPIO_BUT2 |= pin_3_in; // наверно ин
}

// *(uint32_t *)(0x40020C00 + 0xB) |= 0x40000 GPIOD 11
// *(uint32_t *)(0x40020C00 + 0xC) |= 0x200000 GPIOD 12
// *(uint32_t *)(0x40020C00 + 0xD) |=  0x800000 GPIOD 13
// *(uint32_t *)(0x40020400 + 0x02) |= 0x20 //GPIOB
// *(uint32_t *)(0x40020400 + 0x06) |= 0x2000  /GPIOB
// *(uint32_t *)(0x40021000 + 0x02) |= 0x20 //GPIOE
// *(uint32_t *)(0x40018000 + 0x02) |= 0x20 //GPIOG
// *(uint32_t *)(0x40018000 + 0x03) |= 0x80 //GPIOG

