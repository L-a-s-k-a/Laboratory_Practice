#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f411xe.h"
#define RCC_GPIO_en                 (*(uint32_t*)(0x40023800UL+0x30UL)) //включение тактирование на GPIO 
#define GPIOB_moder                 *(uint32_t *)(0x40020400UL + 0x00UL) 
#define GPIOB_ospeed                *(uint32_t *)(0x40020400UL + 0x08UL)
#define GPIOB_pull_up_down          *(uint32_t *)(0x40020400UL + 0x0CUL)      

#define RCC_GPIOB_en                0x02UL //включение тактирования на GPIOB 
#define GPIOB_moder_PIN7            0x4000UL //GPIOB7 на выход 
#define GPIOB_ospeed_PIN7           0x4000UL //GPIOB7 на средная
#define GPIOB_pull_up_down_PIN7     0x00UL  //GPIOB7 отключаем резисторы
#define SET_bit(REG, BIT)          (REG|=BIT)

void GPIO_Ini(void);
void RCC_Init(void);
void IRO_INInt(void);
void SysTick_Init(void);