#include "init.h"

void GPIO_Ini_Manual() {
    // *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02; // Enable clock for GPIOB
    // *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x04; // Enable clock for GPIOC
    *(uint32_t *)(0x40023800UL + 0x30UL) |= 0b0110; // Enable clock for GPIOB и GPIOC

    // Setting up pin 7 GPIOB
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x4000;   // mode OUTPUT
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x4000;   // speed MEDIUM
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x00;     // Disable PU/PD resistors
    *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x800000; // RESET
}

void GPIO_Ini_CMSIS() {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);
}

void GPIO_Ini() {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // LED 1 (PB8)
    *((uint32_t *)(0x40020400UL + 0x00UL)) |= (0x1UL << 16); // mode register
    *((uint32_t *)(0x40020400UL + 0x08UL)) |= (0x1UL << 16); // speed register

    // LED 2 (PB5)
    SET_BIT_CUSTOM(GPIOB_CUSTOM->MODER, GPIO_MODERX_0(5));
    SET_BIT_CUSTOM(GPIOB_CUSTOM->OSPEEDR, GPIO_OSPEEDRX_0(5));

    // LED 3 (PB 6)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE6_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED6_0);
}
