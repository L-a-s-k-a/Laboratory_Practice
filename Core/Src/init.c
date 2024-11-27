#include "D:\MK\aboba\rep\Laboratory_Practice\Core\Inc\init.h"
void LED1_out()
{
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODE3_0); //PA3, указан A0
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_3);
    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEEDR3_0);
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR3_0);
    SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS3);
    return;
}
void LED2_out()
{
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE0_0); //PC0 A1
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_0);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0_0);
    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS0);
    return;
}
void LED3_out()
{
    SET_BIT(GPIOC->MODER, GPIO_MODER_MODE3_0); //PC3 A2
    CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_3);
    SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR3_0);
    CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR3_0);
    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS3);
    return;
}
void LED1_in()
{
    SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR3);
    CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE3_0); //PA3
}
void LED2_in()
{
    SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR3);
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE0_0); //PC0
}
void LED3_in()
{
    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR3);
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE3_0); //PC3
}
void GPIO_Init(){
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); 
}

void GPIO_Init_const_LED_button()
{
    //LEDS
    //PB7
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0); //настройка в output mode
     CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7); //настройка на push pull
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0); //настройка скорости работы на medium
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0); //отключение pull up pull down резисторов
    //PB14
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14_0);
    //PB0
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0);
    //BUTTON
    //PC13
    CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE13_0); //PC13
}