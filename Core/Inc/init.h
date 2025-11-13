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

#define READ_GPIO_B9               (*(uint32_t*)(0x40020400UL+0x10UL) & 0x200UL) // Считивание с порта В9

#define RESET_GPIO_C12             (*(uint32_t*)(0x40020800UL+0x18UL)|= 0x10000000UL) //сброс  BSSR12 для порта С12
#define RESET_GPIO_D2              (*(uint32_t*)(0x40020C00UL+0x18UL)|= 0x40000UL) //сброс  BSSR2 для порта D2
#define RESET_GPIO_B7              (*(uint32_t*)(0x40020400UL+0x18UL)|= 0x800000UL) //сброс  BSSR7 для порта B7
#define SET_bit(REG, BIT)          (REG|=BIT)


void GPIO_Ini(void);
// void dop_test(uint8_t *counter1, uint8_t *button1, uint8_t *prev_state3, uint32_t *fortime2);
void main_task_pin_PB8_9(uint8_t *counter, uint8_t *button, uint8_t *prev_state1, uint8_t *prev_state2);