#include "init.h"

uint8_t led_count = 0;      
uint8_t led_state = 1;      
int blink_period = 100;     

int main(void)
{
    GPIO_Ini(); 

    int blink_timer = 0;    
    int btn1_time = 0;      

    uint8_t btn2_flag = 0;  
    uint8_t btn3_flag = 0;  

    uint32_t btn2_start = READ_BIT(GPIOC->IDR, GPIO_IDR_ID0);
    uint32_t btn3_start_1 = READ_BIT(GPIOC->IDR, GPIO_IDR_ID1);
    uint32_t btn3_start_3 = READ_BIT(GPIOC->IDR, GPIO_IDR_ID3);

    while (1)
    {
        if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) != 0)
        {
            btn1_time = btn1_time + 1;

            if (btn1_time == 100)
            {
                if (led_count > 0)
                {
                    led_count = led_count - 1;
                }
            }
        }
        else
        {
            if (btn1_time > 5 && btn1_time < 100)
            {
                if (led_count < 6)
                {
                    led_count = led_count + 1;
                }
            }
    
            btn1_time = 0;
        }

        if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID0) != btn2_start)
        {
            if (btn2_flag == 0)
            {
                if (blink_period > 20)
                {
                    blink_period = blink_period - 20;
                }
                btn2_flag = 1;
            }
        }
        else
        {
            btn2_flag = 0;
        }

       
        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_ID1) != btn3_start_1) ||
            (READ_BIT(GPIOC->IDR, GPIO_IDR_ID3) != btn3_start_3))
        {
            if (btn3_flag == 0)
            {
                if (blink_period < 300)
                {
                    blink_period = blink_period + 20;
                }
                btn3_flag = 1;
            }
        }
        else
        {
            btn3_flag = 0;
        }

        
        blink_timer = blink_timer + 1;
        if (blink_timer >= blink_period)
        {
            blink_timer = 0;
            if (led_state == 0)
            {
                led_state = 1;
            }
            else
            {
                led_state = 0;
            }
        }


        if (led_state == 1)
        {
            
            if (led_count >= 1) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS0);  else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            if (led_count >= 2) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);  else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            if (led_count >= 3) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS14); else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            if (led_count >= 4) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS1);  else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR1);
            if (led_count >= 5) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS2);  else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR2);
            if (led_count >= 6) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15); else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
        }
        else
        {
        
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR1);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR2);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
        }
        
        for (volatile int i = 0; i < 15000; i++)
        {

        }
    }
}