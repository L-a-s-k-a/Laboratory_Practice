#include <stdint.h>
#include "init.h"

static void delay (unsigned int time) {
    for (unsigned int i = 0; i < time; i++)
        for (volatile unsigned int j = 0; j < 2000; j++);
}

uint8_t buttonState = 1;
uint8_t lastButtonState = 0;
volatile uint8_t counter = 0;
uint8_t led1_state = 0;
uint8_t led2_state = 0;
uint8_t led3_state = 0;
uint8_t led4_state = 0;


int main(void){

    GPIO_Ini();


    while(1){

            buttonState = (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)) ? 1 : 0;
            
            if(lastButtonState == 1 && buttonState == 0){

                counter++;
                if (counter == 7){
                    counter = 1; 
                }
            }

            lastButtonState = buttonState;

            delay(10);

            if(counter == 1){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR8);

                led1_state = 1;
                led2_state = 0;
                led3_state = 0;
                led4_state = 0;


            }
            if (counter == 2){

                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                led2_state = 1;
            }

            if (counter == 3){

                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
                led3_state = 1;
            }
            if (counter == 4){

                SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS8);
                led4_state = 1;
            }

            if (counter == 5){

                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR8);

                led1_state = 0;
                led2_state = 0;
                led3_state = 0;
                led4_state = 0;
            }
            if (counter == 6){

                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
                SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS8);
                led1_state = 1;
                led2_state = 1;
                led3_state = 1;
                led4_state = 1;
            }
                    
            // SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
            // SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            // SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            
        }
        

    




    return 0;
}

