#include <stdint.h>
#include <stdbool.h>
#include "C:\Users\parni\Git_repos\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\STM32F429xx\stm32f429xx.h"
#include "C:\Users\parni\Git_repos\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\stm32f4xx.h"
//#include "C:\Users\parni\Git_repos\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\STM32F411xE\stm32f411xe.h"
//#include "C:\Users\parni\Git_repos\Laboratory_Practice\CMSIS\Devices\STM32F4xx\Inc\stm32f4xx.h"

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define RCC_GPIO_EN     *(uint32_t *)(0x40023800UL + 0x30UL)    // тактирование GPIO портов
#define GPIOC_MODER     *(uint32_t *)(0x40020800UL + 0x00UL)    // выбор режима режима output/input GPIOC
#define GPIOC_OTYPER    *(uint32_t *)(0x40020800UL + 0x04UL)    // выбор режима push-pull/open drain GPIOC
#define GPIOC_OSPEEDR   *(uint32_t *)(0x40020800UL + 0x08UL)    // выбор скорости работы GPIOC
#define GPIOC_PUPDR     *(uint32_t *)(0x40020800UL + 0x0CUL)    // выбор подключения подтягивающих резисторов GPIOC
#define GPIOC_BSRR      *(uint32_t *)(0x40020800UL + 0x018UL)   // bit set/reset GPIOC


#define RCC_GPIOC_EN            0x04UL                          // GPIOC RCC enable
#define MODER_GPIOC_8_OUTP      0x10000UL                       // GPIOC 8 output mode
#define OTYPER_GPIOC_8_PUPL     0x00UL                          // GPIOC 8 push-pull mode
#define OSPEEDR_GPIOC_8_MED     0x10000UL                       // GPIOC 8 medium speed
#define PUPDR_GPIOC_8_NPUPD     0x00UL                          // GPIOC 8 no pull-up, no pull-down
#define BSRR_GPIOC_8_OFF        0x1000000UL                     // GPIOC 8 off


void RCC_Init(void);
void GPIO_Init(void);
void SetLEDState(bool ledB1, bool ledB15, bool ledB14, bool ledB13, bool ledB10, bool ledB4);
void main_task(volatile bool *mode_button, volatile short int *led_button);
void add_task(volatile short int *frequency_button, volatile short int *led_button, volatile int *frequency);
void EXTI15_10_IRQHandler(void);