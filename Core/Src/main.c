#include <../Inc/init.h>

uint8_t button_state_1=0, button_state_2=0;
uint8_t button1_clicks=0, button2_clicks=0;
uint8_t led_state=0, led_num=0, alternative=0;
uint8_t port_C8_state=0;

void check_button_state_1(){
   if(button_state_1==0 && (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0)){
      button_state_1=1;  
      button1_clicks+=1;
      for(int i=0;i<750000;i++);
   }
   if(button_state_1==1 && (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0)){
      button_state_1=0;  
      button1_clicks+=1;
      for(int i=0;i<750000;i++);
   }
}

void check_button_state_2(){
   if(button_state_2==0 && (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0)){
      button_state_2=1;  
      button2_clicks+=1;
      for(int i=0;i<500000;i++);
   }
   if(button_state_2==1 && (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0)){
      button_state_2=0;  
      button2_clicks+=1;
      for(int i=0;i<500000;i++);
   }
}

void check_alternative(){
   if(alternative==0 && button2_clicks%6==5){
      alternative=1;
      button2_clicks=0;
   }
   if(alternative==1 && button2_clicks%6==5){
      alternative=0;
      button2_clicks=0;
   }
}

void reconfigure_pin_C8_to_in(){
   CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE8_0);
   CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_8);
   CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);
   CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR8_0);
}
void reconfigure_pin_C8_to_out(){
   SET_BIT(GPIOC->MODER, GPIO_MODER_MODE8_0);
   CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_8);
   SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR8_0);
   CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR8_0);
}

int main(void)
{
   GPIO_Init_Self_Def();

   while (1){
      check_alternative();
      if(alternative==0){
         if(port_C8_state==0){
            reconfigure_pin_C8_to_in();
            port_C8_state=1;
         }
         check_button_state_1();
         check_button_state_2();
         if(button1_clicks%5==1)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
         if(button1_clicks%5==2)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
         if(button1_clicks%5==3)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
         if(button1_clicks%5==4)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS12);
         if(button1_clicks%5==0){
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR12);
         }
      }
      if(alternative==1){
         if(port_C8_state==1){
            reconfigure_pin_C8_to_out();
            port_C8_state=0;
            SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS8);
         }
         check_button_state_2();
      }
   }
}