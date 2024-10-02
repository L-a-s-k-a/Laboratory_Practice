#include "../Inc/init.h"

int main(void){
    GPIO_Ini_Self();
    GPIO_Ini_CMSIS();
    //adress to LED 1 (pin14)
    while(1){
        //SET_BIT(GPIOB->BSRR, GPIO_BSRR);
        /*if(READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0){
            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); //Установка единицы в 7-ой бит регистра ODR
            }
            else{
            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET); //Установка нуля в 7-ой бит регистра ODR
            }*/
        if(READ_BIT_Self(GPIOC_IDR, GPIOC_IDR_PIN13) != 0){
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); //Установка единицы в 0-ой бит регистра ODR
            }
            else{
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET); //Установка нуля в 0-ой бит регистра ODR
            }
    }
}