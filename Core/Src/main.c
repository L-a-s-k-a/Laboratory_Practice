#include "../Inc/init.h"

uint8_t counter = 0;
uint8_t flag = 0;

int main(void){
    GPIO_Ini_Self();
    GPIO_Ini_CMSIS();
    //address of LED 1 (GPIOB_pin14)
    //address of LED 2 (GPIOB_pin0)
    //address of LED 3 (GPIOB_pin7)
    //address of LED 4 (GPIOB_pin8)
    //address of Button (GPIOB_pin0)
    while(1){  
        if(READ_BIT_Self(GPIOC_IDR, GPIOC_IDR_PIN0) != 0){
            if(flag == 0){
                flag = 1;
                counter++;
            }           
        }
        else if(flag == 1){
            flag = 0;
        }
        if(counter%6 == 0)//включение всех светодиодов на 6-ое нажатие
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x4000UL;//включение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); //включение светодиода 0-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);//включение светодиода 7-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8);//включение светодиода 8-го пина GPIOB
            counter = 0;
        }
        else if (counter%6 == 5)//выключение всех светодиодов
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);//выключение светодиода 0-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//выключение светодиода 7-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//выключение светодиода 8-го пина GPIOB
        }
        else if (counter%6 == 1)//LED 1 //14
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x4000UL;//включение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);//выключение светодиода 0-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//выключение светодиода 7-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//выключение светодиода 8-го пина GPIOB
        }
        else if (counter%6 == 2)//LED 2 /0
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET);//включение светодиода 0-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//выключение светодиода 7-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//выключение светодиода 8-го пина GPIOB
        }
        else if (counter%6 == 3)//LED 3 / 7
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);//выключение светодиода 0-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);//включение светодиода 7-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//выключение светодиода 8-го пина GPIOB
        }
        else if (counter%6 == 4)//LED 4  /8
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);//выключение светодиода 0-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//выключение светодиода 7-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8);//включение светодиода 8-го пина GPIOB
        }         
    }
}