#include "init.h"

void GPIO_Init() {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // LED 1 (PB8) прямое обращение к памяти
    *((uint32_t *)(0x40020400UL + 0x00UL)) |= (0x1UL << 16); // mode register
    *((uint32_t *)(0x40020400UL + 0x08UL)) |= (0x1UL << 16); // speed register

    // LED 2 (PB9) обращение через самописные макросы
    BIT_SET(GPIOB_CUSTOM->MODER, GPIO_MODERX_0(9));
    BIT_SET(GPIOB_CUSTOM->OSPEEDR, GPIO_OSPEEDRX_0(9));

    // LED 3 (PB10) обращение через CMSIS
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE10_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED10_0);
}
