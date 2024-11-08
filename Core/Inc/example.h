#include "init.h"

void CMSIS_Version() {
    GPIO_Ini_CMSIS();
    while (1) {
        if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0) {
            WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS7);
        } else {
            WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR7);
        }
    }
}

void Manual_Version() {
    GPIO_Ini_Manual();
    while (1) {
        if ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x2000UL) != 0) {
            *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x80;
        } else {
            *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x800000;
        }
    }
}