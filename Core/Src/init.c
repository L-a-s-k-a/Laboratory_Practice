#include "init.h"

void GPIO_Ini(void)
{
    // Включение тактирования портов GPIOB и GPIOC
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    // Настройка пинов светодиодов (PB0, PB1, PB2, PB7, PB14, PB15) в режим вывода (Output mode)
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 |
                          GPIO_MODER_MODER7_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0);

    // Настройка на работу в режиме Push-Pull
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT0 | GPIO_OTYPER_OT1 | GPIO_OTYPER_OT2 |
                             GPIO_OTYPER_OT7 | GPIO_OTYPER_OT14 | GPIO_OTYPER_OT15);

    // Настройка скорости работы пинов на среднюю (Medium speed)
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0 | GPIO_OSPEEDER_OSPEEDR1_0 | GPIO_OSPEEDER_OSPEEDR2_0 |
                            GPIO_OSPEEDER_OSPEEDR7_0 | GPIO_OSPEEDER_OSPEEDR14_0 | GPIO_OSPEEDER_OSPEEDR15_0);

    // Отключение подтягивающих резисторов PU/PD для светодиодов
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD0_0 | GPIO_PUPDR_PUPD1_0 | GPIO_PUPDR_PUPD2_0 |
                            GPIO_PUPDR_PUPD7_0 | GPIO_PUPDR_PUPD14_0 | GPIO_PUPDR_PUPD15_0);

    // Включение стягивающих резисторов (Pull-Down) для внешних кнопок PC0 и PC3
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD0_1 | GPIO_PUPDR_PUPD3_1);
}