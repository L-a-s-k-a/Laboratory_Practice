#include <stdint.h>
//#include <init.h>
#include <stm32f429xx.h>

void run_advance(void);
void run_cmsis_ver(void);
void run_simple_ver(void);
//void run_define_ver(void); закомментировал так как имена дефайнов совпадают с cmsis дефайнами

work_mode = 0;

int main(void){
    //run_cmsis_ver()
    //run_simple_ver()
    //run_define_ver()
}

void run_cmsis_ver(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

    GPIOB->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER7 | GPIO_MODER_MODER14);
    GPIOB->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER7_0 | GPIO_MODER_MODER14_0);

    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR8_0 | GPIO_PUPDR_PUPDR9_0);

    while(1) {
        if(work_mode == 1) {
            GPIOB->BSRR = GPIO_BSRR_BS9; //ON control diod
        } else {
            if((GPIOB->IDR & GPIO_IDR_ID9) == 0) {
                GPIOB->BSRR = GPIO_BSRR_BS0; //ON GREEN
            } else {
                GPIOB->BSRR = GPIO_BSRR_BR0; // OFF GREEN
            }
            GPIOB->BSRR |= GPIO_BSRR_BR9; //OFF C_DIOD
        }

        if((GPIOC->IDR & GPIO_IDR_ID13) != 0) {
            GPIOB->MODER &= ~(GPIO_MODER_MODER9); //ReGister input for PB9
            GPIOB->PUPDR |= GPIO_PUPDR_PUPD9_0;
            work_mode = 0;
            GPIOB->BSRR |= GPIO_BSRR_BS14; //ON RED
        } else {
            GPIOB->BSRR |= GPIO_BSRR_BR14; // OFF RED
        }

        if((GPIOB->IDR & GPIO_IDR_ID8) == 0) {
            GPIOB->MODER |= GPIO_MODER_MODER9_0;
            work_mode = 1;
            GPIOB->BSRR |= GPIO_BSRR_BS7; //ON BLUE
        } else {
            GPIOB->BSRR |= GPIO_BSRR_BR7;
        }
    }
}

void run_simple_ver(void) {

    *(uint32_t*)(0x40023800UL + 0x30UL) |= (1UL << 1) | (1UL << 2); //Clock on port B and C

    *(uint32_t*)(0x40020400UL + 0x00UL) |= 1UL; //Register moder - Output mode for PB0(GREEN)
    *(uint32_t*)(0x40020400UL + 0x00UL) |= (1UL << 14); //Register moder - Output mode for PB7(BLUE)
    *(uint32_t*)(0x40020400UL + 0x00UL) |= (1UL << 28); //Register moder - Output mode for PB14(RED)

    *(uint32_t*)(0x40020400UL + 0x0CUL) |= (1UL << 16); //Register PUPDR pull-up for PB8
    *(uint32_t*)(0x40020400UL + 0x0CUL) |= (1UL << 18); //Register PUPDR pull-up for PB9

    while(1) {
        if(work_mode == 1) {
                *(uint32_t*)(0x40020400UL + 0x18UL) |= (1UL << 9); //ON DIODcontrol
            } else {
                if(((*(uint32_t*)(0x40020400UL + 0x10UL)) & (1UL << 9)) == 0) { //if button3 ON
                    *(uint32_t*)(0x40020400UL + 0x18UL) |= 1UL; //ON DIOD(green)
                } else {
                    *(uint32_t*)(0x40020400UL + 0x18UL) |= (1UL << 16);
                }
                *(uint32_t*)(0x40020400UL + 0x18UL) |= (1UL << 25); //Off DIODcontrol
            }

        if(((*(uint32_t*)(0x40020800UL + 0x10UL)) & (1UL << 13)) != 0) { //if button1 ON
            *(uint32_t*)(0x40020400UL + 0x00UL) &= ~(1UL << 18); //Register moder - Input mode for PB9
            *(uint32_t*)(0x40020400UL + 0x0CUL) |= (1UL << 18); //Register PUPDR pull-up for PB9
            work_mode = 0;
            *(uint32_t*)(0x40020400UL + 0x18UL) |= (1UL << 14); //ON DIOD(red)
        } else {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= (1UL << 30);
        }

        if(((*(uint32_t*)(0x40020400UL + 0x10UL)) & (1UL << 8)) == 0) { //if button2 ON
            *(uint32_t*)(0x40020400UL + 0x00UL) |= (1UL << 18); //Register moder - Output mode for PB9
            work_mode = 1;
            *(uint32_t*)(0x40020400UL + 0x18UL) |= (1UL << 7); //ON DIOD(blue)
        } else {
            *(uint32_t*)(0x40020400UL + 0x18UL) |= (1UL << 23);
        }
    }
}

/*
void run_define_ver(void) {
    RCC |= RCC_GPIOB | RCC_GPIOC; //Clock GPIOB GPIOC

    GPIOB_MODER |= PB0_SET; //PB0 OUTPUT
    GPIOB_MODER |= (1UL << 14); //PB7 OUTPUT
    GPIOB_MODER |= (1UL << 28); //PB14 OUTPUT

    GPIOB_PUPDR |= (1UL << 16); //Pull Up PB8
    GPIOB_PUPDR |= (1UL << 18); //Pull Up PB9

    while(1) {
        if(work_mode == 1) {
            GPIOB_SET_RESET |= PB9_SET; //ON control diod
        } else {
            if((GPIOB_READ & B3_SET) == 0) {
                GPIOB_SET_RESET |= PB0_SET; //ON GREEN
            } else {
                GPIOB_SET_RESET |= (1UL << 16); // OFF GREEN
            }
            GPIOB_SET_RESET |= (1UL << 25); //OFF C_DIOD
        }

        if((GPIOC_READ & B1_SET) != 0) {
            GPIOB_MODER &= ~(1UL << 18); //Refister input for PB9
            GPIOB_PUPDR |= (1UL << 18);
            work_mode = 0;
            GPIOB_SET_RESET |= PB14_SET; //ON RED
        } else {
            GPIOB_SET_RESET |= (1UL << 30); // OFF RED
        }

        if((GPIOB_READ & B2_SET) == 0) {
            GPIOB_MODER |= (1UL << 18);
            work_mode = 1;
            GPIOB_SET_RESET |= PB7_SET; //ON BLUE
        } else {
            GPIOB_SET_RESET |= (1UL << 23);
        }
    }
}
*/