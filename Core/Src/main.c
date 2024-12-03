#include <init.h>
#include <stdint.h>

int button_state_1=0;
int button_state_2=0;
int button1_clicks=0;
int button2_clicks=0;
int alternative=0;
int port_C0_state=0; // 0 - out;   1 - in

void check_alternative()
{
        if(alternative==0 && button2_clicks%6==5){
                alternative=1;
                button2_clicks=0;
        }
        if(alternative==1 && button2_clicks%6==5){
                alternative=0;
                button2_clicks=0;
        }
}
void check_button_state_1()
{
        if(alternative==0 && button_state_1==0 && READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0)!=0){
                button_state_1=1;  
                button1_clicks+=1;
                for(int i=0;i<750000;i++);
        }
        if(button_state_1==1 && READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0)!=0){
                button_state_1=0;  
                button1_clicks+=1;
                for(int i=0;i<750000;i++);
        }
}
void check_button_state_2()
{
        if(button_state_2==0 && READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0){
                button_state_2=1;  
                button2_clicks+=1;
                for(int i=0;i<750000;i++);
        }
        if(button_state_2==1 && READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13)!=0){
                button_state_2=0;  
                button2_clicks+=1;
                for(int i=0;i<750000;i++);
        }
}
void reconfigure_pin_C0_to_in(){
        CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODE0_0);
        CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_0);
        CLEAR_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
        CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0_0);
}
void reconfigure_pin_C0_to_out(){
        SET_BIT(GPIOC->MODER, GPIO_MODER_MODE0_0);
        CLEAR_BIT(GPIOC->OTYPER, GPIO_OTYPER_OT_0);
        SET_BIT(GPIOC->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
        CLEAR_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0_0);
}

int main(void)
{
        GPIO_Ini();
        GPIO_Ini_Self_Def();
        GPIO_Ini_CMSIS();
        while(1){
                check_alternative();
                if(alternative==0){
                        if(port_C0_state==0){
                                reconfigure_pin_C0_to_in();
                                port_C0_state=1;
                        }
                        check_button_state_1();
                        check_button_state_2();
                        if(button1_clicks%4==1)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);
                        if(button1_clicks%4==2)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                        if(button1_clicks%4==3)SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14);
                        if(button1_clicks%4==0){
                                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
                                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
                        }
                }
                if(alternative==1){
                        if(port_C0_state==1){
                                reconfigure_pin_C0_to_out();
                                port_C0_state=0;
                        }
                        check_button_state_2();
                        if(button2_clicks%2==1)SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS0);
                        if(button2_clicks%2==0)SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR0);

                }
        }
}