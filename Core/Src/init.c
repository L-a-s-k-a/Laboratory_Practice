#include "../Inc/init.h"

//проверить и переделать
void GPIO_Ini_Self(void)
{
    //set LED 2 (pin0)
    RCC_AHB1ENR |= RCC_GPIOB_EN | RCC_GPIOC_EN; //Включение тактирования портов GPIOB и GPIOC
    GPIOB_MODER |= GPIOB_MODE_PIN0_OUT; //Настройка работы 0-го пина GPIOB в режиме вывода сигнала (Output mode)
    GPIOB_OTYPER |= GPIOB_OTYPE_PIN0_PP; //Настройка на Push-Pull работу 0-го пина GPIOB (Output Push-Pull)
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN0_MID; //Настройка скорости работы 0-го пина GPIOB на среднюю
    GPIOB_PUPDR |= GPIOB_PUPDR_PIN0_NOPUPD; //Отключение PU/PD резисторов для 0-го пина GPIOB
}
void GPIO_Ini_CMSIS(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); //Включение тактирования портов GPIOB и GPIOC
    //set LED 3 (pin7)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0); //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT7);//Настройка на Push-Pull работу 0-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);//Настройка скорости работы 7-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD7);//Отключение PU/PD резисторов для 7-го пина GPIOB
    CLEAR_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//сброс BSRR
}