#include "init.h"
#include "interrupt.h"

int main(void)
{
    // Инициализация
    /*
    RCC_Init();           // Настройка тактирования

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN);

    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER9_1); //Настраиваем пин на альтернативный режим 
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk); //Настраиваем пин на максимальную скорость работы 
    MODIFY_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL9_Msk, 0x0); //Выбираем тип альтернативной функции – Выход MCO2 

    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER8_1); //Настраиваем пин на альтернативный режим 
    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED8_Msk); //Настраиваем пин на максимальную скорость работы 
    MODIFY_REG(GPIOA->AFR[1], GPIO_AFRH_AFSEL8_Msk, 0x0); //Выбираем тип альтернативной функции – Выход MCO1
    */

    //ну тут всё и так должно быть понятно
    RCC_Init();
    GPIO_Init();
    SysTick_Init(); 
    Interrupt_Init();

    while(1)
    {
        if(ledMode == 0)
            Update_Mode0();
        else
            Update_Mode1();

    }
    return 0;
}
