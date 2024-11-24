#include <init.h>

void GPIO_1(void){

      SET_BIT(RCC_GPIO_EN,RCC_GPIOB_EN | RCC_GPIOC_EN | RCC_GPIOC_EN);
   // *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x06; //Включение тактирования портов GPIOB и GPIOC 
     SET_BIT(GPIOB_MODER,GPIOB_MODE_PIN7_OUT | GPIOB_MODE_PIN14_OUT);
    //*(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
      SET_BIT(GPIOB_OTYPER,GPIOB_OTYPE_PIN7_PP | GPIOB_OTYPE_PIN14_PP );
    //*(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //Настройка на PushPull работу 7-го пина GPIOB (Output Push-Pull) 
    SET_BIT(GPIOB_OSPEEDR, GPIOB_OSPEED_PIN7_MID | GPIOB_OSPEED_PIN14_MID);
    //*(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; //Настройка скорости работы 7-го пина GPIOB на среднюю 
    SET_BIT(GPIOB_PUPDR,GPIOB_PUPDR_PIN7_NOPUPD | GPIOB_PUPDR_PIN14_NOPUPD);
    
    //*(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00; //Отключение PU/PD резисторов для 7-го пина GPIOB 
}
