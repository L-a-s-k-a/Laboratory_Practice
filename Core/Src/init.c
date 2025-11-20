#include "../Inc/init.h"

void GPIO_Init_Memory(void)
{
    *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x02UL + 0x04UL;
    
    *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x4000UL; 
    *(uint32_t *)(0x40020400UL + 0x04UL) &= ~0x80UL; 
    *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x4000UL; 
    
    *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x800000UL; 
}

void GPIO_Init_CMSIS(void)
{
  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN | 
                        RCC_AHB1ENR_GPIOCEN | 
                        RCC_AHB1ENR_GPIOGEN | 
                        RCC_AHB1ENR_GPIODEN); //设置按钮


  SET_BIT(GPIOB->MODER, GPIO_MODER_MODER14_0); //pb14引脚设置为输出
  SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);         //pb14引脚输出设置为0

  SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
  SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);

  SET_BIT(GPIOD->MODER, GPIO_MODER_MODER7_0);
  SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR7);

  SET_BIT(GPIOB->MODER, GPIO_MODER_MODER10_0);
  SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR10);
}