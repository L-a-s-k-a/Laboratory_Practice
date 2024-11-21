#include "init.h"

void GPIO_Ini() {
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // LED 1 (PB8) прямое обращение к памяти
    *((uint32_t *)(0x40020400UL + 0x00UL)) |= (0x1UL << 16); // mode register
    *((uint32_t *)(0x40020400UL + 0x08UL)) |= (0x1UL << 16); // speed register

    // LED 3 (PB 6) обращение через CMSIS
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE6_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED6_0);
}

void RCC_Ini() {
    
}

void ITR_Ini() {

}