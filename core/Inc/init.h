#include<stdint.h>
#include "../../CMSIS/Devices/stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"


#define RCC_AHB1ENR                *(uint32_t *)(0x40023800UL + 0x30UL) /*RCC地址偏移量加AHB1总线(启动GPIO时钟)地址偏移量*/

#define RCC_AHB1ENR_GPIOBEN        0X02 /*地址偏移量*/
#define RCC_AHB1ENR_GPIOCEN        0X04

#define GPIOB_MODER                *(uint32_t *)(0x40020400UL + 0x00UL)  /*GPIOB地址偏移量加MODER地址偏移量*/
#define GPIOB_OSPEEDR              *(uint32_t *)(0x40020400UL + 0x08UL)
#define GPIOB_BSRR                 *(uint32_t *)(0x40020400UL + 0x18UL)

#define GPIO_PIN_7_OUT            0x4000UL  
#define GPIO_PIN_7_MED            0x4000UL 
#define GPIO_PIN_7_SET            0x80UL /*蓝色led */
#define GPIO_PIN_1_SET            0x00004000 /*红色led */
#define GPIO_PIN_2_SET            0x00000001 /*绿色led */
#define GPIO_PIN_7_RESET          0x800000UL 

#define LED2_ON            0x80UL /*蓝色led */
#define LED1_ON            0x00004000 /*红色led */
#define LED3_ON            0x00000001 /*绿色led */
#define LED1_OFF        (0x80UL << 16)     // Pb7
#define LED2_OFF        (0x00004000UL << 16) // Pb14
#define LED3_OFF        (0x00000001UL << 16) // Pb0
#define BUTTON1_PIN               0x2000UL 

// // 定义 LED 对应的 GPIO BSRR 位
// #define LED1_ON (1UL << 2)  // BSRR2
// #define LED2_ON (1UL << 6)  // BSRR6
// #define LED3_ON (1UL << 1)  // BSRR1
// #define LED4_ON (1UL << 4)  // BSRR4
// #define LED5_ON (1UL << 3)  // BSRR3
// #define LED6_ON (1UL << 5)  // BSRR5

// #define LED1_OFF (1UL << 18) // BR2
// #define LED2_OFF (1UL << 22) // BR6
// #define LED3_OFF (1UL << 17) // BR1
// #define LED4_OFF (1UL << 20) // BR4
// #define LED5_OFF (1UL << 19) // BR3
// #define LED6_OFF (1UL << 21) // BR5

// // 定义按钮引脚（假设 PC13 和 PC6 分别接按钮1和按钮2）
// #define BUTTON1_PIN (1UL << 13) // PC13
// #define BUTTON2_PIN (1UL << 6)  // PC6

#define GPIOC_IDR                  *(uint32_t *)(0x40020800UL + 0x10UL)  /*GPIOC地址偏移量加GPIOx_IDR地址偏移量*/
#define GPIO_PIN_3               0x40UL 
#define GPIO_PIN_13               0x2000UL 

#define READ_BIT_SELF(REG,BIT) ((REG) & (BIT))
#define SET_BIT_SELF(REG,BIT) ((REG) |= (BIT))

void GPIO_Ini_Self_Def(void);
void GPIO_Ini_CMSIS(void);