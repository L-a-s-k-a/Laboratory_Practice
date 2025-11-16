#include "stdint.h"
#include "init.h"

uint8_t flag=0;
uint8_t vibor=0x1;
uint8_t count=1;
uint8_t click=0;
uint8_t start=1;

void Diod(int diod, int bit)
{
    unsigned int sbit=0;
    unsigned int rbit=0;
    if(diod==1)
    {
        sbit=GPIO_BSRR_BS15;
        rbit=GPIO_BSRR_BR15;
    }
    else if(diod==2)
    {
        sbit=GPIO_BSRR_BS13;
        rbit=GPIO_BSRR_BR13;
    }
    else if(diod==3)
    {
        sbit=GPIO_BSRR_BS12;
        rbit=GPIO_BSRR_BR12;
    }
    else if(diod==4)
    {
        sbit=GPIO_BSRR_BS10;
        rbit=GPIO_BSRR_BR10;
    }
    else if(diod==5)
    {
        sbit=GPIO_BSRR_BS11;
        rbit=GPIO_BSRR_BR11;
    }
    else if(diod==6)
    {
        sbit=GPIO_BSRR_BS0;
        rbit=GPIO_BSRR_BR0;
    }
    if(bit==1)
    {
        if(diod==1)
        {
            *(uint32_t*)(0x40020400UL+0x18UL)|=(0x1UL<<15);
        }
        else
        {
            SET_BIT(GPIOB->BSRR, sbit);
        }
    }
    else
    {
        if(diod==1)
        {
            *(uint32_t*)(0x40020400UL+0x18UL)|=(0x1UL<<31);
        }
        else
        {
            SET_BIT(GPIOB->BSRR, rbit);
        }
    }
}

void Start()
{
    if((vibor&0x1)==0x1)
    {
        Diod(1, 1);
    }
    else if((vibor&0x1)!=0x1)
    {
        Diod(1, 0);
    }
    if((vibor&0x2)==0x2)
    {
        Diod(2, 1);
    }
    else if((vibor&0x2)!=0x2)
    {
        Diod(2, 0);
    }
    if((vibor&0x4)==0x4)
    {
        Diod(3, 1);
    }
    else if((vibor&0x4)!=0x4)
    {
        Diod(3, 0);
    }
    if((vibor&0x8)==0x8)
    {
        Diod(4, 1);
    }
    else if((vibor&0x8)!=0x8)
    {
        Diod(4, 0);
    }
    if((vibor&0x10)==0x10)
    {
        Diod(5, 1);
    }
    else if((vibor&0x10)!=0x10)
    {
        Diod(5, 0);
    }
    if((vibor&0x20)==0x20)
    {
        Diod(6, 1);
    }
    else if((vibor&0x20)!=0x20)
    {
        Diod(6, 0);
    }
}


void Click()
{
    for(int i=0;i<count;i++)
    {
        vibor=vibor<<1;
        if((vibor&0x40)==0x40)
        {
            vibor=((vibor&(~0x40)))|0x01;
        }
    }
    if(count==6)
    {
        if(vibor==0x0)
        {
            vibor=0x3F;
        }
        else if(vibor==0x3F)
        {
            vibor=0x0;
        }
    }
    Start();
}

int main(void)
{
    GPIO_Ini();
    while(1)
    {
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_8)==0)
        {
            if(flag!=2)
            {
                flag=1;
            }
        }
        else
        {
            flag=0;
        }
        if(flag==1)
        {
            flag=2;
            count++;
            while ((vibor&0x1)==0x1)
            {
                vibor=vibor<<1;
                if((vibor&0x40)==0x40)
                {
                    if(vibor!=0x7E)
                    {
                        vibor=((vibor&(~0x40))|0x1);
                    }
                    else
                    {
                        vibor=((vibor&(~0x40)));
                    }
                }
            }
            vibor|=0x1;
            start=1;
            if(count>6)
            {
                vibor=0x1;
                count=1;
            }
        }

        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_9)==0)
        {
            if(click!=2)
            {
                click=1;
            }
        }
        else
        {
            click=0;
        }
        if(click==1)
        {
            click=2;
            if(start)
            {
                start=0;
                Start();
            }
            else
            {
                Click();
            }
        }
    }
}