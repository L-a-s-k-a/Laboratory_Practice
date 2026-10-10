#include "init.h"


void SystemInit(void) {
    
}

int main(void) {
    
    SET_BIT(RCC_GPIO_EN, (RCC_GPIOB_EN | RCC_GPIOC_EN));

     
    GPIOB_MODER &= ~0x3000C003UL; 
    GPIOB_MODER |=  0x10004001UL; 

    
    GPIOC_MODER &= ~0x000000F3UL; 

    
    GPIOC_PUPDR &= ~0x000000F3UL; 
    GPIOC_PUPDR |=  0x000000A2UL; 

    uint8_t port_mode = 0;

    
    WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET);
    WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN14_RESET);
    WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);

    while (1) {
  
        if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN0) != 0) {
            port_mode = 0;
            
           
            GPIOC_MODER &= ~0x000000C0UL;
            WRITE_REG(GPIOC_BSRR, GPIOC_BSRR_PIN3_RESET);
            
            
            WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET);
            WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN14_RESET);
        } else {
           
            WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET);
        }

       
        if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN2) != 0) {
            port_mode = 1;
            
            
            GPIOC_MODER &= ~0x000000C0UL;
            GPIOC_MODER |=  0x00000040UL;
            WRITE_REG(GPIOC_BSRR, GPIOC_BSRR_PIN3_SET);
            
            
            WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET);
            WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET);
        } else {
           
            WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN14_RESET);
        }
        
      
        if (port_mode == 0) {
            
            if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN3) != 0) {
                WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET);
            } else {
                WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);
            }
        } else {
            
            WRITE_REG(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);
        }
    }
}




 
