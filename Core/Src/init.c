#include "../Inc/init.h"

//проверить и переделать
void GPIO_Ini_Self(void)
{
    //set LED 1 (pin14)
    RCC_AHB1ENR |= RCC_GPIOB_EN | RCC_GPIOC_EN; //Включение тактирования портов GPIOB и GPIOC
    *(uint32_t*)(0x40020400UL) |= 0x10000000UL;//Настройка работы 14-го пина GPIOB в режиме вывода сигнала (Output mode)
    *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x0000UL; //Настройка на Push-Pull работу 14-го пина GPIOB (Output Push-Pull)
    *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x10000000UL; //Настройка скорости работы 14-го пина GPIOB на среднюю
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x0000UL; //Отключение PU/PD резисторов для 14-го пина GPIOB
    *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;//выключение светодиода 14-го пина GPIOB
    //set LED 2 (pin0)
    GPIOB_MODER |= GPIOB_MODE_PIN0_OUT; //Настройка работы 0-го пина GPIOB в режиме вывода сигнала (Output mode)
    GPIOB_OTYPER |= GPIOB_OTYPE_PIN0_PP; //Настройка на Push-Pull работу 0-го пина GPIOB (Output Push-Pull)
    GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN0_MID; //Настройка скорости работы 0-го пина GPIOB на среднюю
    GPIOB_PUPDR |= GPIOB_PUPDR_PIN0_NOPUPD; //Отключение PU/PD резисторов для 0-го пина GPIOB
    GPIOB_BSRR |= GPIOB_BSRR_PIN0_RESET;//выключение светодиода 0-го пина GPIOB
}
void GPIO_Ini_CMSIS(void)
{
    //SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); //Включение тактирования портов GPIOB и GPIOC
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