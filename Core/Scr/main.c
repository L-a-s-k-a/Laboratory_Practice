#include "../Inc/init.h"

uint8_t but1 = 0;
uint8_t but2 = 0;
uint8_t but3 = 0;
uint32_t but4 = 0;
uint8_t led1 = 0;
uint8_t led2 = 0;
uint8_t led3 = 0;
int main()
{
    uint8_t mode = 1;
    Register_Init();
    Led_Init();
    while(1)
    {
        if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0)
        {
            delay();
            but4++;
            if (mode < 5)
            mode = mode + 1;
            else
            {
                mode = 1;
            }
        }
        switch (mode)
        {
            case 1: All_Ports_Input(); break;
            case 2: First_Port_Output(); break;
            case 3: Second_Port_Output(); break;
            case 4: Third_Port_Output(); break;
            case 5: All_Ports_Output(); break;
        }
        if ((READ_BIT(GPIOE->IDR, GPIO_IDR_IDR_0) != 0) && mode !=5 && mode != 2)
        {
            but1 = 1;
            S_BIT(GPIO_B_BSRR, GPIO_B_BSRR_PB0_SET);
            led1 = 1;
        }
        else
        {
            but1 = 0;
            S_BIT(GPIO_B_BSRR, GPIO_B_BSRR_PB0_RESET);
            led1 = 0;
        }
        if ((READ_BIT(GPIOD->IDR, GPIO_IDR_IDR_7) != 0) && mode != 5 && mode != 3)
        {
            but2 = 1;
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80;
            led2 = 1;
        }
        else
        {
            but2 = 0;
            *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000;
            led2 = 0;
        }
        if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_8) != 0 && mode != 5 && mode != 4)
        {
            but3 = 1;
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            led3 = 1;
        }
        else
        {
            but3 = 0;
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            led3 = 0;
        }
    }
}
