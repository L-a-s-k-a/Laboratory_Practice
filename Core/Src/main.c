#include <init.h>
#include <stdint.h>

int button_state=0;
int led_state=0;
int led_num=0;

void check_button_state(){
        if(button_state==0 && READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0){
                button_state=1;  
                led_num+=1;
                for(int i=0;i<500000;i++);
        }
        if(button_state==1 && READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0){
                button_state=0;  
                led_num+=1;
                for(int i=0;i<500000;i++);
        }
}

int main(void){
        GPIO_Ini();
        GPIO_Ini_Self_Def();
        GPIO_Ini_CMSIS();
        while(1){
                check_button_state();
                if(led_num==1)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
                if(led_num==2)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                if(led_num==3)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
                if(led_num==4)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8);
                
                if(led_num==5){
                        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
                        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);
                        led_num=0;
                }
        }

        // *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02;
        // adsas
        // *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000;
        // *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000;
        // *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000;
        

        // while(1){
        //         *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80;
        // }
        // while(1){
        //         if(READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0){
        //                 SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET);
        //         }else{
        //                 SET_BIT(GPIOB_BSRR, GPIOB_BSTT_PIN7_RESET);
        //         }    
        // }
}