

#include "main.h"

// Объявляем переменные

volatile uint32_t delayTimerValue = 0;
volatile uint8_t num_leds = 1;


uint32_t last_button2_time = 0;
uint8_t btn2_z = 1;
uint8_t count =4, count_button_1 =0;
uint8_t mode =0;


void SysTick_Handler(void)
{
	delayTimerValue--;
}

void delayMs(uint32_t delay)
{
	delayTimerValue = delay;

	while(delayTimerValue);
}

void change_port_B(void)
{
	if( mode == 1)
	{
	    GPIOB->MODER &= ~(GPIO_MODER_MODER10);
		 mode =0;
	}
	else
	{
		GPIOB->MODER &= ~(GPIO_MODER_MODER10);
		GPIOB->MODER |= GPIO_MODER_MODE10_0;
		 mode =1;
	}
}
void Leds_count (uint8_t h)
{
  switch (h) {

  case 0:	Led_Red_ON; /*Led_Yellow_OFF; Led_Green_OFF;*/	break;
  case 1:   Led_Yellow_ON;/* Led_Red_OFF; Led_Green_OFF;*/  break;
  case 2:   Led_Green_ON;/* Led_Red_OFF;Led_Yellow_OFF; */   break;
  case 3:   all_leds_off;  break;
 }
}
int main (void)
{
  RCC_init();
  GPIO_init();
  delayMs(1000);

   while(1)
   {
	   uint32_t current_time = SysTick->VAL;

	   // Чтение кнопок
	   uint8_t btn2_state = (GPIOA->IDR & GPIO_IDR_IDR_10) ? 0 : 1;


       if (btn2_state && (current_time - last_button2_time >Button_Delay))
       {
           last_button2_time = current_time;
           delayMs(50);

          if(btn2_z == 1)
          {
           if(count_button_1 == 5)
           {
        	   change_port_B();
        	   count_button_1 = 0;
           }
           btn2_z = 0;
           count = (count +1)%4;
           Leds_count(count);
           count_button_1++;
          }
       }
       if(btn2_state == 0) {btn2_z = 1;}
   }
}

