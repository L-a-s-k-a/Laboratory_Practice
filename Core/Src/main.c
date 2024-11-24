#include <stdint.h>


#include <stdint.h> 
#include <init.h> 

int main(void) { 

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
    while(1){ 
        if((READ_BIT(GPIOC_IDR ,GPIOC_IDR_PIN13  ) != 0)){ 
            SET_BIT(GPIOB_BSRR ,GPIOB_BSRR_PIN7_RESET | GPIOB_BSRR_PIN14_RESET);
           // *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80; //Установка единицы в 7-ой бит регистра ODR 
        } 
        else{ 
             SET_BIT(GPIOB_BSRR ,GPIOB_BSRR_PIN7_SET | GPIOB_BSRR_PIN14_SET);
            //*(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000; //Установка нуля в 7-ой бит регистра ODR 
        } 
    } 
} 


// int main(void) {
//     *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02; //Включение тактирования портов GPIOB и GPIOC
//     *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; //Настройка работы 7-го пина GPIOB в режиме вывода сигнала (Output mode)
//     *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //Настройка на Push-Pull работу 7-го пина GPIOB (Output Push-Pull)
//     *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; //Настройка скорости работы 7-го пина GPIOB на среднюю
//     *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00; //Отключение PU/PDрезисторов для 7-го пина GPIOB
//     while(1){
//         if((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x2000UL) != 0){
//             *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80; //Установка единицы в 7-ой бит регистра ODR
//             }
//             else{
//                 *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000; //Установка нуля в 7-ой бит регистра ODR
//                 }
//             }
// }