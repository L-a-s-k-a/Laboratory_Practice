#include "init.h"

int main(void)
{
    GPIO_Ini(); //Инициализация поротв GPIO
    while (1)
    {
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!= 0){
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Установкаединицы в 7-ой бит регистра ODR
        }
        else{
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Установка нуля в 7-ой бит регистра ODR
        }
    }
}
