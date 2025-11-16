#include "init.h"
#include "C:/Users/Lenovo Legion/Desktop/Laboratory_Practice-main/Laboratory_Practice/Tools/xpack-arm-none-eabi-gcc-13.2.1-1.1/arm-none-eabi/include/stdint.h"

void GPIO_Ini(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN|RCC_AHB1ENR_GPIOCEN); //Включение тактирования портов GPIOB и GPIOC
    //input 8
    SET_BIT(GPIOC->MODER, 0x0);
    //9
    SET_BIT(GPIOC->MODER, 0x0);
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD8_0|GPIO_PUPDR_PUPD9_0); //Bключение PU/PD резисторов для 7-го пина GPIOB
    //output 7
    *(uint32_t*)(0x40020400UL+0x00UL)|=(0x01<<30); //moder15
    *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //otyper15
    *(uint32_t*)(0x40020400UL + 0x08UL) |= (0x01<<30); //speedmed15
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= (0x01<<30); //pup15
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE13_0|GPIO_MODER_MODE12_0|GPIO_MODER_MODE10_0|GPIO_MODER_MODE11_0|GPIO_MODER_MODE0_0); //Настройка работы 13, 12, 10, 11, 0 пинов GPIOB в режиме вывода сигнала (Output mode)
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_13|GPIO_OTYPER_OT_12|GPIO_MODER_MODE10_0|GPIO_MODER_MODE11_0|GPIO_MODER_MODE0_0); //Настройка на PushPull работу 13, 12, 10, 11, 0 пинов GPIOB (Output Push-Pull)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR13_0|GPIO_OSPEEDER_OSPEEDR12_0|GPIO_MODER_MODE10_0|GPIO_MODER_MODE11_0|GPIO_MODER_MODE0_0); //Настройка скорости работы 13, 12, 10, 11, 0 пинов GPIOB на среднюю
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD13_0|GPIO_PUPDR_PUPD12_0|GPIO_MODER_MODE10_0|GPIO_MODER_MODE11_0|GPIO_MODER_MODE0_0); //Отключение PU/PD резисторов для 13, 12, 10, 11, 0 пинов GPIOB
}