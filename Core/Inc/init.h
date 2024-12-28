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


#define BUTTON1_PIN        (1U << 13) // PC13
#define BUTTON2_PIN        (1U << 6)  // PC6
#define LED_PORT           GPIOB
#define BUTTON_PORT        GPIOC

#define GPIOC_IDR                  *(uint32_t *)(0x40020800UL + 0x10UL)  /*GPIOC地址偏移量加GPIOx_IDR地址偏移量*/
#define GPIO_PIN_6               0x00000040UL 
#define GPIO_PIN_13               0x2000UL 

#define READ_BIT_SELF(REG,BIT) ((REG) & (BIT))
#define SET_BIT_SELF(REG,BIT) ((REG) |= (BIT))

void GPIO_Ini_Self_Def(void);
void GPIO_Ini_CMSIS(void);