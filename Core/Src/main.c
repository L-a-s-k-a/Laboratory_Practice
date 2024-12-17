#include <../Inc/init.h>
uint8_t button1=0, button2=0, button3=0;
int button1_clicks=0;
int counter=0;
int flag=0;

int main(void){  

GPIO_Init_CMSIS();
while (1){
   if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0 && flag==0) {
      flag=1;
      for(int i=0;i<750000;i++);
      if( counter==0){
         CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODE0_Msk);
         SET_BIT(GPIOB->MODER, GPIO_MODER_MODE12_0);
         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS12);
         counter+=1;
      }
      else if(counter==1){
         CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODE7_Msk);
         SET_BIT(GPIOB->MODER, GPIO_MODER_MODE11_0);
         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS11);
         counter+=1;
      }
      else if(counter==2){
         CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODE14_Msk);
         SET_BIT(GPIOB->MODER, GPIO_MODER_MODE10_0);
         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS10);
         counter+=1;
      }
      else if(counter==3){
         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR12);
         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR11);
         SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);
        counter=0;
      }}
   if(READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)==0 ) {
      flag=0;
   }
   if(counter==0){ 
      SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
      SET_BIT(GPIOB->MODER, GPIO_MODER_MODE7_0);
      SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
      CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODE12_Msk);
      CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODE11_Msk);
      CLEAR_BIT(GPIOB->MODER, GPIO_MODER_MODE10_Msk);
   }
     
   if(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_12)!=0 ){
      SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
   }
   if(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_12)==0){
      SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
   }
   if(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_11)!=0 ){
      SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
   }
   if(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_11)==0){
      SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
   }
   if(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_10)!=0 ){
      SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
   }
   if(READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_10)==0){
      SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
   }
   }
}

