#include "init.h"

#define GPIO_IDRX(X) 1UL << X

// BSRR редактировать только с WRITE_REG()
#define GPIO_BSRR_BSX(X) 1UL << X
#define GPIO_BSRR_BRX(X) 1UL << (X + 16)

uint8_t pin[] = {1, 2, 3};
uint8_t led[] = {0, 7, 14}; // диоды на плате
uint8_t j = 0;
uint8_t b = 0, bp = 0;   // состояние кнопки режима
uint8_t b1 = 0, b1p = 0; // состояния кнопки 1
uint8_t b2 = 0, b2p = 0; // состояния кнопки 2
uint8_t b3 = 0, b3p = 0; // состояния кнопки 3

void set_modes(uint8_t pins) {
    for (int i = 0; i < 3; ++i) {
        uint8_t mask = (pins >> i) & ~0x1;
        set_mode(pin[i], mask);
    }
}

void set_mode(uint8_t pin, uint8_t mode) {
    uint8_t offset = pin * 2;
    if (READ_BIT(GPIOC->MODER, 0x3UL << offset) != 0) {
        if (mode) return;
        CLEAR_BIT(GPIOC->MODER, 0x1UL << offset);
    } else {
        if (!mode) return;
        SET_BIT(GPIOC->MODER, 0x1UL << offset);
    }
}

void change_mode() {
    switch(j++) {
    case 0: set_modes(0b001); break;
    case 1: set_modes(0b010); break;
    case 2: set_modes(0b100); break;
    case 3: set_modes(0b111); break;
    case 4: set_modes(0b000); break;
    case 5: j = 0; break;
    }
}

void process_pbpin(uint8_t pin) {
    if (READ_BIT(GPIOB->IDR, GPIO_IDRX(pin)) != 0) {
        WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BSX(pin));
    } else {
        WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BRX(pin));
    }
}

void process_pbbtn(uint8_t pin) {

}

int main_func_3() {
    while (1) {
        b = (READ_BIT(GPIOC->IDR, GPIO_IDRX(13)) != 0);
        if (READ_BIT(GPIOB->IDR, GPIO_IDRX(1)) != 0) {

        }
        b2 = (READ_BIT(GPIOB->IDR, GPIO_IDRX(2)) != 0);
        b3 = (READ_BIT(GPIOB->IDR, GPIO_IDRX(3)) != 0);

        process_pbpin(1);
        process_pbpin(2);
        process_pbpin(3);
        
        if (bp == 0 && b == 1) change_mode();
        bp = b;
    }
    return 0;
}