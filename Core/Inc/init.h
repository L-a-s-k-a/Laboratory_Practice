#ifndef INIT_H
#define INIT_H

#include<stdint.h>
// #include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
// #include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f429xx.h"
#include "stm32f4xx.h"
#include "stm32f429xx.h"

//void GPIO_Ini(void);
void GPIOC10_Set_Output(void);
void GPIOC10_Set_Input(void);
void GPIOC11_Set_Output(void);
void GPIOC11_Set_Input(void);
void GPIOC12_Set_Output(void);
void GPIOC12_Set_Input(void);
void GPIOC13_Set_Input(void);
void Green_LED_ON(void);
void Blue_LED_ON(void);
void Red_LED_ON(void);

#define RCC_AHB1ENR                 *(uint32_t *)(0x40023800UL + 0x30UL)
#define RCC_GPIOB_EN                0x02UL
#define RCC_GPIOC_EN                0x04UL

#define GPIOB_MODER                 *(uint32_t *)(0x40020400UL + 0x00UL)
#define GPIOB_OTYPER                *(uint32_t *)(0x40020400UL + 0x04UL)
#define GPIOB_OSPEEDR               *(uint32_t *)(0x40020400UL + 0x08UL)
#define GPIOB_BSRR                  *(uint32_t *)(0x40020400UL + 0x18UL)
#define GPIOB_PUPDR                 *(uint32_t *)(0x40020400UL + 0x0CUL)

#define GPIOC_IDR                   *(uint32_t *)(0x40020800UL + 0x10UL)


#define GPIOB_OUTPUT_MEDIUM_PIN_7    0x4000UL
#define GPIOB_OUTPUT_MODE_PIN_7      0x4000UL
#define GPIOB_PIN_RESET_7            0x800000UL
#define GPIOB_PIN_SET_7              0x80UL
#define GPIOB_PP_PIN_7               0x80UL


#define GPIOB_OUTPUT_MEDIUM_PIN_14    0x10000000UL
#define GPIOB_OUTPUT_MODE_PIN_14      0x10000000UL
#define GPIOB_PIN_RESET_14            0x40000000UL
#define GPIOB_PIN_SET_14              0x4000UL
#define GPIOB_PP_PIN_14               0x30000000UL
#define GPIOB_OTYPE_PIN_14_PP         0x0000UL  

#define GPIO_PIN_PC12               0x1000UL

#define BIT_SET(REG, BIT)       ((REG) |= (BIT))
#define BIT_READ(REG, BIT)      ((REG) & (BIT))
#define BIT_CLEAR(REG, BIT)     ((REG) &= ~(BIT))

#endif

