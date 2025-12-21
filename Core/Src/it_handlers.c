#include "C:\Users\parni\Git_repos\Laboratory_Practice\Core\Inc\init.h"


extern uint16_t FrequencyTickCount;
extern uint16_t ButtonTickCount;
extern bool led_mode;
extern uint8_t freq_mode;


void SysTick_Handler(void)
{
    FrequencyTickCount++;
    ButtonTickCount++;
}


void EXTI15_10_IRQHandler(void)
{ 
    SET_BIT(EXTI->PR, EXTI_PR_PR12); 
    // при нажатии кнопки обнуляем счетчик
    if (!READ_BIT(GPIOC->IDR, GPIO_IDR_ID12))   
    {
        ButtonTickCount = 0;
    }
    else 
    {
        // Если после отпускание кнопки прошло больше 2 сек., то частота мерцания меняется
        if (ButtonTickCount >= 2000)                           
        {
            freq_mode++;
            if (freq_mode >= 3)
            {
                freq_mode = 0;
            }
            FrequencyTickCount = 0;
            ButtonTickCount = 0;
        }
        // Если после отпускание кнопки прошло менее 2 сек., то меняется режим работы
        else if (ButtonTickCount >= 100 && ButtonTickCount <= 2000)  
        {
            led_mode = !led_mode;
            freq_mode = 0;
            FrequencyTickCount = 0;
            ButtonTickCount = 0;

            
        }
    }
}

