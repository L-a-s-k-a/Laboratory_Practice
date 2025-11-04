#include "init.h"

int main(void)
{
    GPIO_Init(); 

    while (1)
    {
        // ВКЛЮЧЕНИЕ СВЕТОДИОДА (PC13 = LOW)
        *(uint32_t*)(GPIOC_BASE + GPIOC_BSRR_OFFSET) = PC13_RESET; 
    }
}
