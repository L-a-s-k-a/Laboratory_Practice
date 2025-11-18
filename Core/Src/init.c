#include "../Inc/init.h"


void GPIO_Ini_1(void){
    SET_BIT(RCC_GPIO_EN, RCC_GPIOB_EN | RCC_GPIOC_EN); // Включить GPIO B и GPIO C
    SET_BIT(GPIOB_MODER, GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN0_OUT | GPIOB_MODE_PIN14_OUT); // Установить в режим вывода
    SET_BIT(GPIOB_OTYPER, GPIOB_OTYPE_PIN7_PP | GPIOB_OTYPE_PIN0_PP | GPIOB_OTYPE_PIN14_PP); // Установить двухтактный выход
    SET_BIT(GPIOB_OSPEEDR, GPIOB_OSPEED_PIN7_MID | GPIOB_OSPEED_PIN0_MID | GPIOB_OSPEED_PIN14_MID); // Установите среднюю скорость.
    SET_BIT(GPIOB_PUPDR, GPIOB_PUPDR_PIN7_NOPUPD | GPIOB_PUPDR_PIN0_NOPUPD | GPIOB_PUPDR_PIN14_NOPUPD); // Отключить подтягивающие/стягивающие резисторы


}
// void GPIO_Init_Memory(void){
//     *(uint32_t*)(0x40023800UL + 0x30UL) | = 0x02UL + 0x04UL; //включение тактирования портов GPIOB
   
//     *(uint32_t*)(0x40020400UL + 0x00UL) | = 0x4000UL; //настройка работы 7-го пина GPIOB в режиме вывода сигнала
//     *(uint32_t*)(0x40020400UL + 0x04UL) &=  ~0x80UL;  //настройка на pp
//     *(uint32_t*)(0x40020400UL + 0x08UL) | = 0x4000UL;//настройка скорости
   
//     *(uint32_t*)(0x40020400UL + 0x18UL) | = 0x800000UL; //настройка резистора


// }

// void GPIO_Init_CMSIS(void)
// {
//     SET_BIT(RCC->AHP1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIODEN);//включение GPIOB C G
 

//     SET_BIT(GPIOB->MJDER, GPIO_MODER_MODER14_0);//настройка режима работы 14 на выход
//     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);       //установка 0 на выходе 14
//     SET_BIT(GPIOB->MODER, GPIO_MODER_MODER0_0);
//     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);

//     SET_BIT(GPIOG->MODER, GPIO_MODER_MODER7_0);
//     SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);


// }
