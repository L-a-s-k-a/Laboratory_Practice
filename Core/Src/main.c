#include "D:\MK\aboba\rep\Laboratory_Practice\Core\Src\init.c"
#include "D:\MK\aboba\rep\Laboratory_Practice\Core\Inc\init.h"
uint8_t flag;
int main(void)
{
    GPIO_Init();
    GPIO_Init_const_LED_button();
    GPIOA_button_OUT();
    while(1)
    {
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0)!=0)
        {
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR3);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
        }
        else
        {
            SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS3);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
        }
        /*
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS3);
        uint32_t i = 0;
        while(i < 100000)
        {
            i++;
        }
        i = 0;
        SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR3);
        while(i < 100000)
        {
            i++;
        }
        i = 0;
        */
    }
}
/*#include "../Inc/main.h"

int main(void)
{s
    //PB7
    // RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02;
    
    //*SET_BIT(GPIOB->MODER, GPIO_MODER_MODER7_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT7);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD7);
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);*/
/*
    *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000;
    *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00;
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000;
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00;
   // *(uint32_t*)(0x40020400UL + 0x14UL) |= 0x80;
    *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000;
    //PB14
    *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x10000000;
    *(uint32_t*)(0x400207FFUL + 0x04UL) |= 0x00;
    //SpeedmodeReg
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x10000000;
    //PullUpPulldownReg
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00;
    //Outputdatareg ODR
  //  *(uint32_t*)(0x40020400UL + 0x14UL) |= 0x2000;
    //Outputdatasetresetreg BSRR
    *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000;  
    while(1)
    {
        *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80;
        *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x4000;
        uint32_t i = 0;
        while(i < 10000000)
        {
            i++;
        }
        i = 0;
        while(i < 10000000)
        {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000;
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000;
        }
        // SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
*/
