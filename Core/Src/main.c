#include <stdint.h>
#include "Init.h"

uint8_t mode, mode_prev = 0; // 10000000 & (1 << 7)
uint8_t count = 4;
// uint8_t state = 0b00000111;//0 - output / 1 - input
/*  
state (uint8_t):
    1   0   0   0   0   0   0   0
    b1  b2  b3  l1  l2  l3
*/
int main(void){
    // GPIOB_RCC_ON;
    // GPIOC_RCC_ON;
    //  *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x02; // включение тактирования GPIOB
    //  *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x04; // включение тактирования GPIOC
    //  *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x4000;   // 7пин на вывод
    //  *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x00;     // 7пин push-pull
    //  *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x4000;   // 7пин средняя скорость
    //  *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x800000; // 7пин - 0 (0DR)
    GPIO_Init();
    GPIO_led_init();
    GPIO_button_input();
    //GPIO_button_output();
    while (1)
    {
        // if (((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x2000)) != 0){
        //     *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x80; // 7пин - 1 (0DR)
        // }
        // else *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x800000; // 7пин - 0 (0DR)
        // if(READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0){
        //     SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET); //Установка 1 в 7 бит регистра
        // }
        // else{
        //     SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET); //Установка 0 в 7 бит регистра
        // }
        // if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0){
        //     flag = 1;
        //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Установка 1 в 7 бит регистра
        // }
        // else{
        //     flag = 0;
        //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Установка 0 в 7 бит регистра
        // }
        // SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8); //Установка 1 в 7 бит регистра
        // if(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) !=  0){
        //     flag = 1;
        //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Установка 1 в 7 бит регистра
        // }
        // else{
        //     flag = 0;
        //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Установка 0 в 7 бит регистра
        // }

        //SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
        mode = (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0);
        if (mode && mode_prev){
            count += 1;
            switch (count) {
            case 1: button1_input(); break;
            case 2: button2_input(); break;
            case 3: button3_input(); break;
            case 4: GPIO_button_input(); break;
            case 5: GPIO_button_output(); break;
            case 6: count = 1; break;
            }
        }
        mode_prev = mode;

    // if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_12) == 0 && but1 == 1)
    // {
    //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8);
    // }
    // else
    // {
    //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);
    // }
    // if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_15) == 0 && but2 == 1)
    // {
    //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5);
    // }
    // else
    // {
    //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR5);
    // }
    // if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_9) == 0 && but3 == 1)
    // {
    //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS6);
    // }
    // else
    // {
    //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR6);
    // }
    
}

}