#ifndef __INIT_H
#define __INIT_H

#include <stdint.h>

// 寄存器地址定义
#define RCC_AHB1ENR                *(uint32_t *)(0x40023800UL + 0x30UL)
#define GPIOB_MODER                *(uint32_t *)(0x40020400UL + 0x00UL)
#define GPIOB_OSPEEDR              *(uint32_t *)(0x40020400UL + 0x08UL)
#define GPIOB_BSRR                 *(uint32_t *)(0x40020400UL + 0x18UL)
#define GPIOC_IDR                  *(uint32_t *)(0x40020800UL + 0x10UL)

// LED引脚定义
#define GPIO_PIN_7_OUT            0x4000UL  
#define GPIO_PIN_7_MED            0x4000UL 
#define GPIO_PIN_7_SET            0x80UL
#define GPIO_PIN_7_RESET          0x800000UL 

#define LED1_ON            0x00004000  // PB14 红色
#define LED2_ON            0x80UL      // PB7  蓝色  
#define LED3_ON            0x00000001  // PB0  绿色
#define LED1_OFF           (0x00004000UL << 16)
#define LED2_OFF           (0x80UL << 16)
#define LED3_OFF           (0x00000001UL << 16)

// 按钮引脚定义
#define GPIO_PIN_6         0x00000040UL
#define GPIO_PIN_13        0x2000UL

// 宏定义
#define READ_BIT_SELF(REG, BIT) ((REG) & (BIT))
#define SET_BIT_SELF(REG, BIT) ((REG) |= (BIT))

// 函数声明
void GPIO_Ini_Self_Def(void);
void GPIO_Ini_CMSIS(void);
void update_leds(uint8_t led_index, uint8_t count);

#endif __INIT_H 





