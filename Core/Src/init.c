#include "init.h"

void GPIO_Ini(void){
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); //Включение тактирования портов GPIOB и GPIOC
    
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0); //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7); //Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0); //Настройка скорости работы 7-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0); //Отключение PU/PDрезисторов для 7-го пина GPIOB

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0); //Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14); //Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0); //Настройка скорости работы 14-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14_0); //Отключение PU/PDрезисторов для 14-го пина GPIOB

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE8_0); //Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_8); //Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0); //Настройка скорости работы 14-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR8_0); //Отключение PU/PDрезисторов для 14-го пина GPIOB

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE9_0); //Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_9); //Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR9_0); //Настройка скорости работы 14-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR9_0); //Отключение PU/PDрезисторов для 14-го пина GPIOB
    
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE10_0); //Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_10); //Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR10_0); //Настройка скорости работы 14-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR10_0); //Отключение PU/PDрезисторов для 14-го пина GPIOB
    
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0); //Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0); //Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0); //Настройка скорости работы 14-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0); //Отключение PU/PDрезисторов для 14-го пина GPIOB
}