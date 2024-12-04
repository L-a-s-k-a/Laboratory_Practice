#include <stdint.h>
#include "Init.h"

uint8_t mode, mode_prev = 0;
uint8_t count = 4;
uint8_t b1 = 0;
uint8_t b2 = 2;
uint8_t b3 = 4;

int main(void){
    GPIO_led_init();
    GPIO_button_input();
    while (1)
    {
      
        mode = (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_3) != 0);
        if (mode == 1 && mode_prev == 0){
            count += 1;
            switch (count) {
            case 1: button1_output(); break;
            case 2: button2_output(); break;
            case 3: button3_output(); break;
            case 4: GPIO_button_output(); break;
            case 5: GPIO_button_input(); 
                    count = 0; break;
            }
        }
        mode_prev = mode;
        //b1-pb14
        if(!READ_BIT(GPIOB->MODER, GPIO_MODER_MODE12_0) && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_12) != 0){ 
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            b1 = 1;
        }
        else {
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14); //set pb12 
            b1 = 0;
        }
        //b2-pb7
        if(!READ_BIT(GPIOB->MODER, GPIO_MODER_MODE15_0) && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_15) != 0){ 
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            b2 = 3;
            }
        else{
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //set pb15
            b2 = 2;
        }
        //b3-pb0
        if(!(*(uint32_t*)(0x40020400UL + 0x00UL) | 0b10) && (((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x2000)) != 0) != 0){
              *(uint32_t *)(0x40020400UL + 0x18UL) |= 1<<0;
              b3 = 5;
        }
        else {
            *(uint32_t *)(0x40020400UL + 0x18UL) |= 1<<16; //set pb0
            b3 = 4;
        }
             
    }               

}