#include "../Inc/init.h"

void delay(uint32_t time_delay)
{    
    uint32_t i;
    for(i = 0; i < time_delay; i++);
    //32768 KHz crystal oscillator
}

uint8_t counter = 0;
uint8_t flag = 0;

int main(void){
    GPIO_Ini_Self();
    GPIO_Ini_CMSIS();
    //address of LED 1 (GPIOB_pin14)
    //address of LED 2 (GPIOB_pin0)
    //address of LED 3 (GPIOB_pin7)
    //address of LED 4 (GPIOB_pin8)
    //address of Button (GPIOB_pin13)
    while(1){
        if(READ_BIT_Self(GPIOC_IDR, GPIOC_IDR_PIN13) != 0){
            if(flag == 0){
                flag = 1;
                counter++;
                delay(1638);
            }           
        }
        else if(flag == 1){
            flag = 0;
            delay(1638);
        }
        if(counter%6 == 0)//включение всех светодиодов на 6-ое нажатие
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x4000UL;//включение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET); //включение светодиода 0-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//включение светодиода 7-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//включение светодиода 8-го пина GPIOB
            if(counter%4 == 0)
            {
                counter = 0;
            }
        }
        else if (counter%5 == 0)//выключение всех светодиодов
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);//выключение светодиода 0-го пина GPIOB
            CLEAR_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//выключение светодиода 7-го пина GPIOB
            CLEAR_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//выключение светодиода 8-го пина GPIOB
        }
        else if (counter%5 == 1)//LED 1
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x4000UL;//включение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);//выключение светодиода 0-го пина GPIOB
            CLEAR_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//выключение светодиода 7-го пина GPIOB
            CLEAR_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//выключение светодиода 8-го пина GPIOB
        }
        else if (counter%5 == 2)//LED 2
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET);//включение светодиода 0-го пина GPIOB
            CLEAR_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//выключение светодиода 7-го пина GPIOB
            CLEAR_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//выключение светодиода 8-го пина GPIOB
        }
        else if (counter%5 == 3)//LED 3
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);//выключение светодиода 0-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//включение светодиода 7-го пина GPIOB
            CLEAR_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//выключение светодиода 8-го пина GPIOB
        }
        else if (counter%5 == 4)//LED 4
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
            SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);//выключение светодиода 0-го пина GPIOB
            CLEAR_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//выключение светодиода 7-го пина GPIOB
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//включение светодиода 8-го пина GPIOB
        }    
    }
}