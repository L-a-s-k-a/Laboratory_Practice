#include "../Inc/init.h"
#include <stdint.h>

uint8_t flag_PC10 = 0; // для кнопки 2 по умолчанию 0-вход 1-выход
uint8_t flag_PC11 = 0; // для кнопки 3
uint8_t flag_PC12 = 0; // для кнопки 4
uint8_t flag_PC13 = 0; // флаг для кнопки на плате (1)
uint32_t count_PC13 = 0; // счетчик для кол-ва нажатий 
uint8_t button_pressed = 0; // флаг нажатия кнопки
uint8_t button_pressed1 = 0; // флаг нажатия кнопки для защиты от дребезга
uint32_t filter;
// кнопка 1 - PC13 
// кнопка 2 и светодиод  - PC10
// кнопка 3 и светодиод  - PC11
// кнопка 4 и светодиод  - PC12
// светодиоды на плате PB0 - зеленый, PB7 - синий, PB14 - красный

int main(void) {
    //SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN); // включение тактирования портов C и B
    BIT_SET(RCC_AHB1ENR, RCC_GPIOB_EN | RCC_GPIOC_EN);
    // Инициализация светодиодов
    Green_LED_ON();
    Blue_LED_ON();
    Red_LED_ON();
    
    // Инициализация кнопки PC13 (вход)
    GPIOC13_Set_Input();
    
    // Изначально все порты PC10-PC12 как вход
    GPIOC10_Set_Input();
    GPIOC11_Set_Input();
    GPIOC12_Set_Input();

    while (1) {
        
        //Обработка кнопки 1 (PC13) - управление режимами портов 
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)) {
            filter = 2000;
            button_pressed1 = 1;
        }
        else if(filter == 0){
            button_pressed1 = 0;
        }    
        else if(filter > 0){
            filter = filter - 1;
        }

        if(button_pressed == 0 & button_pressed1 == 1) {
            button_pressed = 1;            
            count_PC13++;
        }
        else if(button_pressed1 == 0){
            button_pressed = 0;
        } 
                
        if(count_PC13 == 1) {
            GPIOC10_Set_Output();
            GPIOC11_Set_Input();
            GPIOC12_Set_Input();
            flag_PC10 = 1;
            flag_PC11 = 0;
            flag_PC12 = 0;

        }
        else if(count_PC13 == 2) {
            GPIOC11_Set_Output();
            GPIOC10_Set_Input();
            GPIOC12_Set_Input();
            flag_PC10 = 0;
            flag_PC11 = 1;
            flag_PC12 = 0;
        }
        else if(count_PC13 == 3) {
            GPIOC11_Set_Input();
            GPIOC10_Set_Input();
            GPIOC12_Set_Output();
            flag_PC10 = 0;
            flag_PC11 = 0;
            flag_PC12 = 1;
        }
        else if(count_PC13 == 4) {
            GPIOC10_Set_Output();
            GPIOC11_Set_Output();
            GPIOC12_Set_Output();
            flag_PC10 = 1;
            flag_PC11 = 1;
            flag_PC12 = 1;
        }
        else if(count_PC13 == 5) {
            GPIOC10_Set_Input();
            GPIOC11_Set_Input();
            GPIOC12_Set_Input();
            flag_PC10 = 0;
            flag_PC11 = 0;
            flag_PC12 = 0;
            count_PC13 = 0;
        }
        
        if(flag_PC10 == 1) {  //выход
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10); 
        } 
        else { 
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR10); 
        }

        if(flag_PC10 == 0) { // вход
            if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_10)) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0); 
            } 
            else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0); 
            }
        } 
        
        if(flag_PC11 == 1) { 
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11); 
        } 
        else { 
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR11); 
            
        }

        if(flag_PC11 == 0) {
            if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_11)) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            } 
            else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); 
            }
        } 

        if(flag_PC12 == 1) { 
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12); 
        } 
        else { 
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12); 
            
        }

        if(flag_PC12 == 0) { 
            if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_12)) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14); 
            } 
            else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14); 
            }
        } 
    }
}

