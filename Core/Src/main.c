// Вариант 1
// Собрать схему с включением 3-х светодиодов и 2-х кнопок
// 1) Каждое нажатие включает следующий светодиод, предыдущий
// выключается.
// 2) Каждое нажатие устанавливает количество одновременно
// включаемых светодиодов
// 3) Использовать три метода общения с МК - напрямую, с помощью define, CMSIS


#include "init.h"
#include <stdint.h>

uint32_t led_pin = 1;
uint32_t led_mode = 1;
uint8_t led1 = 0;
uint8_t led2 = 0;
uint8_t led3 = 0;
uint8_t but1 = 0;
uint8_t but2 = 0;

int main(void) 
{
  GPIO_Init();
  while(1) 
  {
    if(BIT_READ(GPIOB_IDR, GPIOB_IDR_PIN11) == 0) 
    {
      but1 = 1;                                                           
      for(int i = 0; i < 1000000; i++) {}   //delay
      led_pin++;
      if(led_pin >= 4 || (led_mode == 3 && led_pin >=3)) 
      {
        led_pin = 1;
      }
    }
    else {
      but1 = 0;
    }

    if(BIT_READ(GPIOB_IDR, GPIOB_IDR_PIN12) == 0) 
    {                                          
      for(int i = 0; i < 1000000; i++) {}  //delay
      led_mode++;
      if(led_mode >= 4) 
      {
        led_mode = 1;
      }
      but2 = 1;
    }
    else {
      but2 = 0;
    }

    switch (led_mode)
    {
    case 1:
      if(led_pin == 1) 
      {
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN8_SET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN9_RESET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN10_RESET);
        led1 = 1;
        led2 = 0;
        led3 = 0;
      }
      else if(led_pin == 2) 
      {
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN8_RESET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN9_SET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN10_RESET);
        led1 = 0;
        led2 = 1;
        led3 = 0;
      }
      else if(led_pin == 3) 
      {
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN8_RESET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN9_RESET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN10_SET);
        led1 = 0;
        led2 = 0;
        led3 = 1;
      }
      break;
    case 2:
      if(led_pin == 1) 
      {
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN8_SET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN9_SET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN10_RESET);
        led1 = 1;
        led2 = 1;
        led3 = 0;
      }
      else if(led_pin == 2) 
      {
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN8_RESET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN9_SET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN10_SET);
        led1 = 0;
        led2 = 1;
        led3 = 1;
      }
      else if(led_pin == 3) 
      {
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN8_SET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN9_RESET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN10_SET);
        led1 = 1;
        led2 = 0;
        led3 = 1;
      }
      break;
    case 3:
      if(led_pin == 1) 
      {
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN8_SET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN9_SET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN10_SET);
        led1 = 1;
        led2 = 1;
        led3 = 1;
      }
      else if(led_pin == 2) 
      {
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN8_RESET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN9_RESET);
        BIT_SET(GPIOB_BSRR, GPIOB_BSRR_PIN10_RESET);
        led1 = 0;
        led2 = 0;
        led3 = 0;
      }
      break;
    }
  }
}