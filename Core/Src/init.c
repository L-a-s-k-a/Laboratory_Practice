#include "init.h"

void GPIO_Ini(void){
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOBEN); //Включение тактирования портов GPIOB и GPIOC
    
    SET_BIT(GPIOB->MODER, GPIO_MODER_0); //Настройка работы 7-го пинаGPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7); //Настройка на Push-Pullработу 7-го пина GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0); //Настройка скоростиработы 7-го пина GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0); //Отключение PU/PDрезисторов для 7-го пина GPIOB
}