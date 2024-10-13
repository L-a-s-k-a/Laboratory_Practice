#include "../Inc/init.h"

uint8_t flag = 1;
uint8_t count = 0;
uint8_t reverse = 1;



int main(void){

    GPIO_Ini();
    while(1){     
        if ((READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_6 ) !=0)){
            reverse = !reverse;
            for (int i = 0; i < 500000; i++);   
        }
        if (reverse == 0){
            if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_7 ) !=0){
                count++;
                for (int i = 0; i < 500000; i++);
                if (count > 6){
                    count = 0;
                }
            }
            
            if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) != 0) { 
                count--;
                for (int i = 0; i < 500000; i++);
                if (count > 6) {
                    count = 0; 
                }
            }
        }
        if (reverse == 1){
            if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8 ) !=0){
                count++;
                for (int i = 0; i < 500000; i++);
                if (count > 6){
                    count = 0;
                }
            }
            
            if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_7) != 0) { 
                count--;
                for (int i = 0; i < 500000; i++);
                if (count > 6) {
                    count = 0; 
                }
            }
        }

        if (flag == 1){
            switch (count) {
                case 0:
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR6 | GPIO_BSRR_BR7);
                    break;
                case 1:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6);  
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);
                    break;
                case 2:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6 | GPIO_BSRR_BS7);  
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR5 | GPIO_BSRR_BR13 | GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
                    break;
                case 3:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6 | GPIO_BSRR_BS7); 
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5); 
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13 | GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
                    break;
                case 4:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6 | GPIO_BSRR_BS7); 
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5 | GPIO_BSRR_BS13);  
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0 | GPIO_BSRR_BR1);
                    break;
                case 5:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6 | GPIO_BSRR_BS7);
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5 | GPIO_BSRR_BS13 | GPIO_BSRR_BS0);  
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR1);
                    break;
                case 6:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS6 | GPIO_BSRR_BS7);
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS5 | GPIO_BSRR_BS13 | GPIO_BSRR_BS0 | GPIO_BSRR_BS1);  
                    break;
            }
        }
    }
}