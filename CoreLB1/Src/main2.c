#include "C:\Users\parni\Git_repos\Laboratory_Practice\CoreLB1\Inc\init2.h"




int main(void)
{
    
    RCC_Init();

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER7_0);
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER14_0);
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);


    SET_BIT(GPIOC->MODER, GPIO_MODER_MODER9_1); //Настраиваем пин на альтернативный режим 
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk); //Настраиваем пин на максимальную скорость работы 
    MODIFY_REG(GPIOC->AFR[1], GPIO_AFRH_AFSEL9_Msk, 0x0); //Выбираем тип альтернативной функции – Выход MCO2 

    SET_BIT(GPIOA->MODER, GPIO_MODER_MODER9_1); //Настраиваем пин на альтернативный режим 
    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk); //Настраиваем пин на максимальную скорость работы 
    MODIFY_REG(GPIOA->AFR[1], GPIO_AFRH_AFSEL9_Msk, 0x0); //Выбираем тип альтернативной функции – Выход MCO1
    
    


    while(1)
    {
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
        //main_task(&mode_button, &led_button);
        //add_task(&frequency_button, &led_button, &frequency);
    }
}