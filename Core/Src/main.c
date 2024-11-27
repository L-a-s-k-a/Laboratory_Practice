#include "D:\MK\aboba\rep\Laboratory_Practice\Core\Src\init.c"
#include "D:\MK\aboba\rep\Laboratory_Practice\Core\Inc\init.h"
int main(void)
{
    uint8_t Q = 0;
    uint8_t flag1 = 0;
    uint8_t flag2 = 0;
    uint8_t flag3 = 0;
    GPIO_Init();
    GPIO_Init_const_LED_button();
    LED1_in();
    LED2_in();
    LED3_in();
    while(1)
    {   
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0)
        {
           Q++;
           int i = 0;
           while(i < 1000000)
           {
            i++;
           }
           while(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0)
           {
           }
        switch(Q)
        {
            case 1:
            {
                flag1 = 1;
                LED1_out();
                break;
            }
            case 2:
            {
                flag2 = 1;
                LED2_out();
                break;
            }
            case 3:
            {
                flag3 = 1;
                LED3_out();
                break;
            }
            case 4:
            {
                flag1 = 0;
                flag2 = 0;
                flag3 = 0;
                LED1_in();
                LED2_in();
                LED3_in();
                Q = 0;
                break;
            }
        }
        }
        if(flag1 == 0) //PA3
        {
            if(READ_BIT(GPIOA->IDR, GPIO_IDR_IDR_3)!=0)
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
            }
            else
            if(READ_BIT(GPIOA->IDR, GPIO_IDR_IDR_3)==0)
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
        }
        if(flag2 == 0) //PC0
        {
            if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0)!=0)
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            }
            else
            if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0)==0)
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            }
        }
        if(flag3 == 0) //PC3
        {
            if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_3)!=0)
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14); 
            }
            else
            if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_3)==0)
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14); 
            }
        }    
    }
}
/*#include "../Inc/main.h"
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

int main(void)
{
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
