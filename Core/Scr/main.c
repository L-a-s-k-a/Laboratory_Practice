#include <init.h>
#include <ithandlers.h>
uint16_t GlobalTickCount;
uint8_t flag = 8;
int main(void)
{
    GPIO_Ini();     
    RCC_Ini();      
    EXTI_ITR_Ini(); 
    SysTick_Init(); 
    uint16_t led[6][2];
    for (uint16_t i = 0; i < 6; i++)
    {
        for (uint16_t j = 0; j < 2; j++)
        {
            led[i][j] = 0;
        }
    }
    while (1)
    {
        Clear();
        Work(led);
        if (flag == 0)
        {
            LedPush(led);
            flag = 8;
        }
        else if (flag == 1)
        {
            LedMerz(led);
            flag = 8;
        }
        else if (flag == 2)
        {
            LedOn(led);
            flag = 8;
        }
    }
}

