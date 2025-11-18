#ifndef INIT_H
#define INIT_H
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h" 
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429xx/stm32f429xx.h"
void GPIO_Ini_1(void);
#define BIT_READ(REG, BIT)          ((REG) & (BIT))
#define BIT_SET(REG, BIT)           ((REG) |= (BIT))
#define BIT_CLEAR(REG, BIT)         ((REG) &= ~(BIT))
//Включаем часы B,C
#define RCC_GPIO_EN                 (*(uint32_t*)(0x40023800UL + 0x30UL)) 
#define RCC_GPIOB_EN                0x02UL 
#define RCC_GPIOC_EN                0x04UL 
 //Установите часы
#define GPIOB_MODER                 (*(uint32_t*)(0x40020400UL + 0x00UL)) 
#define GPIOB_OTYPER                (*(uint32_t*)(0x40020400UL + 0x04UL)) 
#define GPIOB_OSPEEDR               (*(uint32_t*)(0x40020400UL + 0x08UL)) 
#define GPIOB_PUPDR                 (*(uint32_t*)(0x40020400UL + 0x0CUL))  
#define GPIOB_BSRR                  (*(uint32_t*)(0x40020400UL + 0x18UL)) 
 //Кнопка «Установить»
#define GPIOC_IDR                   (*(uint32_t*)(0x40020800UL + 0x10UL)) 
//Кнопка 1
#define GPIOC_IDR_PIN13             0x002000UL 
//Кнопка 2
#define GPIOC_IDR_PIN12             0x001000UL  
// LED1
#define GPIOB_MODE_PIN0_OUT        0x001UL 
#define GPIOB_OTYPE_PIN0_PP        0x000000UL 
#define GPIOB_OSPEED_PIN0_MID      0x001UL 
#define GPIOB_PUPDR_PIN0_NOPUPD    0x000000UL
#define GPIOB_BSRR_PIN0_SET        0x001UL 
#define GPIOB_BSRR_PIN0_RESET      0x0010000UL
//LED2
#define GPIOB_MODE_PIN7_OUT         0x004000UL       
#define GPIOB_OTYPE_PIN7_PP         0x000000UL 
#define GPIOB_OSPEED_PIN7_MID       0x004000UL 
#define GPIOB_PUPDR_PIN7_NOPUPD     0x000000UL 
#define GPIOB_BSRR_PIN7_SET         0x000080UL 
#define GPIOB_BSRR_PIN7_RESET       0x800000UL 
//LED3
#define GPIOB_MODE_PIN14_OUT        0x0010000000UL 
#define GPIOB_OTYPE_PIN14_PP        0x000000UL 
#define GPIOB_OSPEED_PIN14_MID      0x0010000000UL 
#define GPIOB_PUPDR_PIN14_NOPUPD    0x000000UL
#define GPIOB_BSRR_PIN14_SET        0x004000UL 
#define GPIOB_BSRR_PIN14_RESET      0x0040000000UL
#endif

