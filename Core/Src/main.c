#include <stdint.h>
#include <init.h>
int main(void){
    RCC_AHB1ENR |= RCC_GPIOBEN | RCC_GPIOCEN;
    while (1){
        if(READ_BIT(GPIOC_IDR, GPIO_PIN_13)!=0){
            SET_BIT(GPIOB_BSRR, GPIO_PIN_7_SET);
        }
        else{
            SET_BIT(GPIOB_BSRR, GPIO_PIN_7_RESET);
        }
        *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80;
    }
}