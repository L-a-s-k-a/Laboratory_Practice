

#include "init.h"


//----------------------------------------
void RCC_init(void)
{

	FLASH->ACR |= FLASH_ACR_LATENCY;

	SET_BIT(RCC->CR,RCC_CR_HSION);

	while(READ_BIT(RCC->CR, RCC_CR_HSIRDY == RESET)) {}

	SET_BIT(RCC->CR,RCC_CR_HSITRIM);

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}
//----------------------------------------
void GPIO_init(void)
{

 // тактирование порта А
  SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOAEN);
 // тактирование порта В
  SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOBEN);

//Инициализация порта для желтого светодиода (GPIOB_PIN_5)

// адреса порта В (0x4002 0400 - 0x4002 07FF)
//  настройка порта на выход
  *(uint32_t*) (0x40020400UL + 0x00UL ) &= ~0x0000C00UL;
  *(uint32_t*) (0x40020400UL + 0x00UL ) |=  0x0000400UL;
// настройка порта режим работы (пул-пуш)
  *(uint32_t*) (0x40020400UL + 0x04UL ) &= ~0x0000C00UL;
// настройка порта скорость работы
  *(uint32_t*) (0x40020400UL + 0x08UL ) &= ~0x0000C00UL;
  *(uint32_t*) (0x40020400UL + 0x08UL ) |=  0x0000400UL;
// настройка порта на подтягивающие резисторы
  *(uint32_t*) (0x40020400UL + 0x0CUL ) &= ~0x0000C00UL;
  *(uint32_t*) (0x40020400UL + 0x0CUL ) |= 0x00UL;
// установка в начальное состояние = 1
  *(uint32_t*) (0x40020400UL + 0x14UL ) |= 0xA0;

//------------------------------------------
  //Инициализация порта для красного светодиода (GPIO_PIN_3)

 CLEAR_BIT(GPIOB->MODER,GPIO_MODER_MODE3);
 SET_BIT(GPIOB->MODER,GPIO_MODER_MODER3_0);
 CLEAR_BIT(GPIOB->MODER,GPIO_OTYPER_OT3);
 CLEAR_BIT(GPIOB->OSPEEDR,GPIO_OSPEEDER_OSPEEDR3);
 CLEAR_BIT(GPIOB->PUPDR,GPIO_PUPDR_PUPD3);
 SET_BIT(GPIOB->BSRR,GPIO_BSRR_BS3);

//------------------------------------------
//Инициализация порта для зеленого светодиода (GPIO_PIN_4)

 GPIOB->MODER &=~GPIO_MODER_MODE4;
 GPIOB->MODER |= GPIO_MODER_MODER4_0;
 GPIOB->MODER &=~GPIO_OTYPER_OT4;
 GPIOB->OSPEEDR &=~GPIO_OSPEEDER_OSPEEDR4;
 GPIOB->PUPDR&=~GPIO_PUPDR_PUPD4;
 GPIOB->BSRR = GPIO_BSRR_BS4;
//------------------------------------------
//Инициализация порта для кнопки 1

  GPIOB->MODER &=~GPIO_MODER_MODE10;
  GPIOB->PUPDR&=~GPIO_PUPDR_PUPD10;
  GPIOB->PUPDR|=GPIO_PUPDR_PUPD10_0;

//------------------------------------------
//Инициализация порта для кнопки 2

 GPIOA->MODER &=~GPIO_MODER_MODE10;
 GPIOA->PUPDR&=~GPIO_PUPDR_PUPD10;
 GPIOA->PUPDR|=GPIO_PUPDR_PUPD10_0;

//------------------------------------------


}
