// #include <stdint.h>
#include "init.h"

int main(void){
    GPIO_Ini_Self_Def();
    GPIO_Ini_CMSIS();
    // *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02;

    // *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000;
    // *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000;
    // *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000;

    while(1){
        if(READ_BIT_SELF(GPIO_IDR, GPIO_PIN_13)!=0){
            SET_BIT_SELF (GPIO_BSRR,GPIO_PIN_7_SET);
        }
        else{
            SET_BIT_SELF(GPIO_BSRR, GPIO_PIN_7_RESET);

        }

        if(READ_BIT(GPIOA->IDR, GPIO_IDR_ID9)!=0){
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);

        }

        else{
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
    }}
}