#include "D:\MK\aboba\rep\Laboratory_Practice\Core\Src\init.c"
#include "D:\MK\aboba\rep\Laboratory_Practice\Core\Inc\init.h"
uint8_t Q = 0;
uint8_t flag1 = 0;
uint8_t flag2 = 0;
uint8_t flag3 = 0;
int main(void)
{
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