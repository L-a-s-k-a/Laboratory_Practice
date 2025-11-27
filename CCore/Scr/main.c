#include "../Inc/init.h"
#include "../Inc/interrupt.h"

int main(void)
{
    GPIO_Init();     // Инициализация портов
    RCC_Init();      // Настройка тактирования
    TIM10_Init();    // Настройка таймера
    IQR_Init();      // Настройка прерывания
    TIM3_PWM_Init(); // Настройка ШИМ TIM3
    TIM1_PWM_Init(); // Настройка ШИМ TIM1
    TIM5_PWM_Init(); // Настройка ШИМ TIM5

    while (1)
    {    
    }
    return 0;
}
