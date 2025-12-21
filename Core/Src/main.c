#include "C:\Users\parni\Git_repos\Laboratory_Practice\Core\Inc\init.h"
#include "C:\Users\parni\Git_repos\Laboratory_Practice\Core\Inc\init.h"

bool led_mode = 0;
uint16_t ButtonTickCount = 0;
uint8_t freq_mode = 0;

int main(void)
{
    RCC_Init();
    GPIO_Init();
    ITR_Init();
    SysTick_Init();
    
    while(1)
    {
        
        //Первый режим мерцания светодиодов
        if (led_mode == 0)
        {
            first_led_mode(freq_mode);
        }
        //Второй режим мерцания светодиодов
        else if (led_mode == 1)
        {
            second_led_mode(freq_mode);               
        }
        
    }
    
}
