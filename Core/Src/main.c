#include "init.h"

int main(void)
{
    GPIO_Ini(); //Инициализация поротв GPIO

    int press_butt = 0;
    int butt_press = 0;

    while (1)
    {
        if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0){
            if (butt_press == 0){
                butt_press = 1;
                press_butt++;
                if (press_butt > 4) {
                    press_butt = 1;
                }
                
            }
            
        }
        else {
            butt_press = 0;
        }
        switch (press_butt)
        {
        case 1:
            if (butt_press) {
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            } else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            break;
        case 2:
            if (butt_press) {
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            } else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            break;
        case 3:
            if (butt_press) {
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
            } else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            break;
        case 4:
            if (butt_press) {
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
            } else {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            }
            break;
        
        default:
        press_butt = 0;
            break;
        }
    
        }
    }
