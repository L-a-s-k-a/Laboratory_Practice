#include "init.h"

uint8_t led[] = {8, 5, 6};  // массив пинов светодиодов
uint8_t flag = 1;           // текущий режим работы кнопки 1
uint8_t b1 = 0, b1p = 0;    // текущее и предыдущее состояния кнопки 1
uint8_t b2 = 0, b2p = 0;    // текущее и предыдущее состояния кнопки 2
uint8_t i = 0;              // индекс диода на который действует кнопка 1

void B1_press() {
    if (i > 2) return;
    if (flag) SET_BIT(GPIOB->ODR, 1UL << led[i++]);
    else CLEAR_BIT(GPIOB->ODR, 1UL << led[i++]);
}

void B2_press() {
    flag = !flag;
    i = 0;
}

int main(void) {
    GPIO_Ini();
    while (1) {
        b1 = (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_6) != 0);
        b2 = (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_7) != 0);
        
        if (b1p == 0 && b1 == 1) B1_press();
        if (b2p == 0 && b2 == 1) B2_press();
        
        b1p = b1;
        b2p = b2;
    }
}