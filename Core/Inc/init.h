#include "../../CMSIS/Devices/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/Inc/stm32f429xx.h"

// 开启时钟 B, C, D
#define RCC_GPIO_EN                 (*(uint32_t*)(0x40023800UL + 0x30UL)) 
#define RCC_GPIOB_EN                0x02UL 
#define RCC_GPIOC_EN                0x04UL 
#define RCC_GPIOD_EN                0x08UL 

// GPIOB 寄存器
#define GPIOB_MODER                 (*(uint32_t*)(0x40020400UL + 0x00UL)) 
#define GPIOB_OTYPER                (*(uint32_t*)(0x40020400UL + 0x04UL)) 
#define GPIOB_OSPEEDR               (*(uint32_t*)(0x40020400UL + 0x08UL)) 
#define GPIOB_PUPDR                 (*(uint32_t*)(0x40020400UL + 0x0CUL))  
#define GPIOB_BSRR                  (*(uint32_t*)(0x40020400UL + 0x18UL)) 

// GPIOC 寄存器
#define GPIOC_MODER                 (*(uint32_t*)(0x40020800UL + 0x00UL)) 
#define GPIOC_OTYPER                (*(uint32_t*)(0x40020800UL + 0x04UL)) 
#define GPIOC_OSPEEDR               (*(uint32_t*)(0x40020800UL + 0x08UL)) 
#define GPIOC_PUPDR                 (*(uint32_t*)(0x40020800UL + 0x0CUL))  
#define GPIOC_BSRR                  (*(uint32_t*)(0x40020800UL + 0x18UL)) 
#define GPIOC_IDR                   (*(uint32_t*)(0x40020800UL + 0x10UL))

// GPIOD 寄存器
#define GPIOD_MODER                 (*(uint32_t*)(0x40020C00UL + 0x00UL)) 
#define GPIOD_PUPDR                 (*(uint32_t*)(0x40020C00UL + 0x0CUL)) 
#define GPIOD_IDR                   (*(uint32_t*)(0x40020C00UL + 0x10UL)) 

// LED 和按钮引脚分配
#define BOARD_LED1_PIN              0  // PB0
#define BOARD_LED2_PIN              7  // PB7
#define BOARD_LED3_PIN              14 // PB14
#define CN8_LED4_PIN                8  // Pb8
#define CN8_LED5_PIN                9  // Pb9
#define CN8_LED6_PIN                10 // Pb10
#define BUTTON1_PIN                 11 // PC11
#define BUTTON2_PIN                 6  // PC6
#define BUTTON3_PIN                 12 // PC12

//#define READ_BIT(REG, BIT)          (REG & BIT) 
//#define SET_BIT(REG, BIT)           (REG |= BIT) 
//#define CLEAR_BIT(REG, BIT)         (REG &= ~BIT) 

void GPIO_Ini(void);
