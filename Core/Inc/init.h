#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "..\..\CMSIS\Devices\stm32f429xx.h"
#include "..\..\CMSIS\Devices\stm32f4xx.h"
#include "..\..\CMSIS\Devices\system_stm32f4xx.h"

#define CLEAR_BIT_CUSTOM(REG, BIT)  REG &= ~BIT
#define READ_BIT_CUSTOM(REG, BIT)   REG &BIT
#define SET_BIT_CUSTOM(REG, BIT)    REG |= BIT
#define GPIOB_ADDRESS       0x40020400UL
#define GPIOB_CUSTOM        ((GPIO_TYPEDEF *) GPIOB_ADDRESS)
#define GPIO_MODERX_0(X)    (0x1UL << (X * 2))
#define GPIO_MODERX_1(X)    (0x2UL << (X * 2))
#define GPIO_OSPEEDRX_0(X)  (0x1UL << (X * 2))
#define GPIO_OSPEEDRX_1(X)  (0x2UL << (X * 2))

typedef struct {
    uint32_t MODER;   // 0x00
    uint32_t OTYPER;  // 0x04
    uint32_t OSPEEDR; // 0x08
    uint32_t PUPDR;   // 0x0C
    uint32_t IDR;     // 0x10
    uint32_t ODR;     // 0x14
    uint32_t BSRR;    // 0x18
    uint32_t LCKR;    // 0x1C
    uint32_t AFRL;    // 0x20
    uint32_t AFRH;    // 0x24
} GPIO_TYPEDEF;

void GPIO_Ini_CMSIS();
void GPIO_Ini_Manual();

// Task #6
void GPIO_Ini();

#endif