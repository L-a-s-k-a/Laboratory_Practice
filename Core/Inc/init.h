#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "stm32f4xx.h"

/* ---- Custom defines for LED2 (PB7, blue) ---- */
#define MY_GPIOB_MODER          (*(volatile uint32_t*)(0x40020400UL + 0x00UL))
#define MY_GPIOB_OTYPER         (*(volatile uint32_t*)(0x40020400UL + 0x04UL))
#define MY_GPIOB_OSPEEDR        (*(volatile uint32_t*)(0x40020400UL + 0x08UL))
#define MY_GPIOB_PUPDR          (*(volatile uint32_t*)(0x40020400UL + 0x0CUL))
#define MY_GPIOB_BSRR           (*(volatile uint32_t*)(0x40020400UL + 0x18UL))

#define MY_PIN7_MODE_OUT        0x00004000UL
#define MY_PIN7_OTYPE_PP_MASK   0xFFFFFF7FUL
#define MY_PIN7_OSPEED_MID      0x00004000UL
#define MY_PIN7_PUPDR_NONE_MASK 0xFFFF3FFFUL
#define MY_PIN7_SET             0x00000080UL
#define MY_PIN7_RESET           0x00800000UL

#define MY_SET_BITS(REG, MASK)  ((REG) |= (MASK))
#define MY_CLR_BITS(REG, MASK)  ((REG) &= (MASK))

void GPIO_Ini(void);
void Leds_Set(uint8_t n);       /* turn on first n LEDs (0..6) */
uint8_t Btn_Raw(uint8_t n);     /* raw level of button n (0..2) */

#endif /* INIT_H */