#include <init.h>
#include <stdint.h>
 
uint8_t NumLed = 1;
uint8_t NumBut = 0;
uint8_t flagAllLed  = 0;

int main(void){
        GPIO_Ini();
        GPIOA_button_OUT();
        On_Led(NumLed, NumBut);

        while(1){
                if((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13))!=0){
                        if(NumLed<3){
                                On_Led(NumLed, NumBut + NumLed);                          
                        }else{
                                if(flagAllLed  == 3){
                                        flagAllLed = 4;
                                } else{
                                        flagAllLed  = 3;
                                }
                                On_Led(flagAllLed, flagAllLed); 
                        }
                        if(NumBut > 2){
                                NumBut = 0;   
                        }   
                        while((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13))!=0){
                                for(int i = 0; i < 300000; i++){}
                        }
                }

                if(READ_BIT(GPIOA->IDR, GPIO_IDR_IDR_3)!=0){
                        if(NumLed < 3){
                                NumLed++;
                        } else{
                                NumLed = 1;
                        }
                        while((READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_3))!=0){
                                for(int i = 0; i < 300000; i++){}
                        }
                }
        }
}