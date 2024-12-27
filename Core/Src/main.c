#include "init.h"

int main(void) {
    GPIO_Ini();

    int current_led = 0;       
    int led_count = 1;         
    int button1_press = 0;     
    int button2_press = 0;     
    int all_leds_on = 1;       
    while (1) {
               if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0) {
            if (button1_press == 0) {
                button1_press = 1;
                if (led_count == 3) {
                   
                    all_leds_on = !all_leds_on;
                } else {
                    current_led++;
                    if (current_led > 2) {
                        current_led = 0;
                    }
                }
            }
        } else {
            button1_press = 0;
        }

        if (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_10) != 0) {
            if (button2_press == 0) {
                button2_press = 1;

                led_count++;
                if (led_count > 3) {
                    led_count = 1;
                }

                if (led_count == 3) {
                    all_leds_on = 1;
                }
            }
        } else {
            button2_press = 0;
        }

        if (led_count == 1) {
            if (current_led == 0) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);  
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14 | GPIO_BSRR_BR0); 
            } else if (current_led == 1) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);  
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7 | GPIO_BSRR_BR0);  
            } else if (current_led == 2) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);   
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7 | GPIO_BSRR_BR14); 
            }
        } else if (led_count == 2) {
            if (current_led == 0) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7 | GPIO_BSRR_BS14); 
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);                
            } else if (current_led == 1) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14 | GPIO_BSRR_BS0); 
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);                
            } else if (current_led == 2) {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0 | GPIO_BSRR_BS7);  
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);                
            }
        } else if (led_count == 3) {
            if (all_leds_on) {
               
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7 | GPIO_BSRR_BS14 | GPIO_BSRR_BS0);
            } else {

                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7 | GPIO_BSRR_BR14 | GPIO_BSRR_BR0);
            }
        }

 
        for (volatile int i = 0; i < 100000; i++) {}
    }
}
