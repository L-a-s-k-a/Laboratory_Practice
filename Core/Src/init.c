#include "init.h"

void GPIO_Init_Mem(void){
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x80UL; // Настройка работы 14-го пина GPIOB в режиме вывода сигнала (output mode)
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x4000UL; // Настройка работы 7-го пина GPIOB в режиме вывода сигнала (output mode)
    *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x01UL;// Настройка работы 0-го пина GPIOB в режиме вывода сигнала (output mode)
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x00UL;   // Настройка на PushPull работу 14-го пина GPIOB (Output Push-Pull)
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x80UL; // Настройка скорости работы 14-го пина GPIOB на среднюю
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x4000UL; // Настройка скорости работы 7-го пина GPIOB на среднюю
    *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x01UL; // Настройка скорости работы 7-го пина GPIOB на среднюю
    *(uint32_t *)(0x40020400UL + 0x0CUL) |= 0x00000000UL;   // Отключение PU/PD резисторов для 14-го пина GPIOB
    *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x02; //Включение тактирования портов GPIOB и GPIOC
}

void GPIO_Init_Self_Def(void){
    // RCC_GPIO_EN |= RCC_GPIOB_EN + RCC_GPIOC_EN; // Включение
    // GPIOB_MODER |= GPIOB_MODE_PIN0_OUT;         // Настройка работы 0-го пина
    // GPIOB_MODER |= GPIOB_MODE_PIN7_OUT;         // Настройка работы 7-го пина
    // GPIOB_MODER |= GPIOB_MODE_PIN14_OUT;         // Настройка работы 7-го пина
    // GPIOB_OTYPER |= GPIOB_OTYPE_PIN0_PP;        // Настройка на Push-Pull
    // GPIOB_OTYPER |= GPIOB_OTYPE_PIN7_PP;        // Настройка на Push-Pull
    // GPIOB_OTYPER |= GPIOB_OTYPE_PIN14_PP;        // Настройка на Push-Pull
    // GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN0_MID;     // Настройка скорости
    // GPIOB_PUPDR |= GPIOB_PUPDR_PIN0_NOPUPD;     // Отключение PU/PD
    // GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN7_MID;     // Настройка скорости
    // GPIOB_PUPDR |= GPIOB_PUPDR_PIN7_NOPUPD;     // Отключение PU/PD
    // GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN14_MID;     // Настройка скорости
    // GPIOB_PUPDR |= GPIOB_PUPDR_PIN14_NOPUPD;     // Отключение PU/PD
}

void GPIO_Init_CMSIS(void){
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE12_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_12);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR12_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR12_0);

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_7);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR0_0);

    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);
    CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR14_0);

}