#include "init.h"
void GPIO_Init(void) {
     RCC->AHB1ENR |=RCC_AHB1ENR_GPIOBEN |RCC_AHB1ENR_GPIOEEN|RCC_AHB1ENR_GPIOAEN;
     
}
int main(void)
{
    DWT_Init();
    GPIO_Init();
    INIT_SVETODIOD();
    
    while(1){
        MoveCW(GPIO_1,Number1_GPIO,10000,600);
        delay_ms(2000);
        MoveCCW(GPIO_2,Number2_GPIO,10000,600);
        delay_ms(2000);
    }
}
