#include  <init.h>

uint8_t LedState;

int main(void)
{   
    GPIO_Ini_Self_Def();
    GPIO_Ini_CMSIS();
    
    while (1)
    {
        if(READ_BIT_SELF(GPIOC_IDR, GPIO_PIN_13) !=0){
            SET_BIT_SELF(GPIOB_BSRR, GPIO_PIN_7_SET);
        }
        else{
            SET_BIT_SELF(GPIOB_BSRR, GPIO_PIN_7_RESET);
        }

        if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) !=0){
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
        }
        else{
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
        }
        
    }
}