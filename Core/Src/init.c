#include <init.h>

void GPIO_Ini(void){
        SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIOAEN);

        *(uint32_t*)(0x40020400UL) |= 0x10000000UL;
        *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x0000UL; 
        *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x10000000UL; 
        *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x0000UL; 
        *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL;

        GPIOB_MODER |= GPIOB_MODE_PIN0_OUT; 
        GPIOB_OTYPER |= GPIOB_OTYPE_PIN0_PP; 
        GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN0_MID; 
        GPIOB_PUPDR |= GPIOB_PUPDR_PIN0_NOPUPD; 
        GPIOB_BSRR |= GPIOB_BSRR_PIN0_RESET;

        SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0); 
        CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT7);
        SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR7_0);
        CLEAR_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPDR7_0);
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
}

void GPIOA_button_OUT(){
        CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE3_0); //A0
        CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_3);
        CLEAR_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEEDR3_0);
        CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR3_0);
}

void Ch_Led_On(int NumS){
        if (NumS == 0){
                SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET);
        }else if (NumS == 1){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
        }else if (NumS == 2){
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
        }
}

void Ch_Led_Off(){
        SET_BIT_Self(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);  
}

void On_Led(int NumLed, int NumBut){
        Ch_Led_Off();
        for(int i = NumBut; i < (NumBut + NumLed); i++){
             Ch_Led_On(i);  
        }
}