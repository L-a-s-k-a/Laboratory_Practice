#include "../Inc/init.h"

void GPIO_Ini_CMSIS(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); //Включение тактирования портов GPIOB и GPIOC
    //set LED 3 (pin7)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0); //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT7);//Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);//Настройка скорости работы 7-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);//Отключение PU/PD резисторов для 7-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);//сброс BSRR, выключение светодиода 7-го пина GPIOB
    //set LED 4 (pin8)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE8_0); //Настройка работы 8-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT8);//Настройка на Push-Pull работу 8-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);//Настройка скорости работы 8-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR8_0);//Отключение PU/PD резисторов для 8-го пина GPIOB
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);//сброс BSRR, выключение светодиода 8-го пина GPIOB
}