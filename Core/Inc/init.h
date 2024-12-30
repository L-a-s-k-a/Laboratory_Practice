#include "../../CMSIS/Devices/stm32f4xx.h" 
#include "../../CMSIS/Devices/stm32f429xx.h" 
#include "../../CMSIS/Devices/system_stm32f4xx.h" 
 


// #include <stdint.h>

// // 基地址
// #define RCC_BASE            0x40023800UL
// #define RCC_GPIOB_EN        0x02UL 
// #define RCC_GPIOC_EN        0x04UL
// #define GPIOB_BASE          0x40020400UL
// #define GPIOC_BASE          0x40020800UL

// // 寄存器偏移
// #define MODER_OFFSET        0x00UL
// #define OTYPER_OFFSET       0x04UL
// #define OSPEEDR_OFFSET      0x08UL
// #define PUPDR_OFFSET        0x0CUL
// #define IDR_OFFSET          0x10UL
// #define BSRR_OFFSET         0x18UL
// #define AHB1ENR_OFFSET      0x30UL

// // 寄存器指针
// #define RCC_AHB1ENR         (*(volatile uint32_t *)(RCC_BASE + AHB1ENR_OFFSET))
// #define GPIOB_MODER         (*(volatile uint32_t *)(GPIOB_BASE + MODER_OFFSET))
// #define GPIOB_BSRR          (*(volatile uint32_t *)(GPIOB_BASE + BSRR_OFFSET))
// #define GPIOC_MODER         (*(volatile uint32_t *)(GPIOC_BASE + MODER_OFFSET))
// #define GPIOC_IDR           (*(volatile uint32_t *)(GPIOC_BASE + IDR_OFFSET))
// #define GPIOB_SPEEDR        (*(volatile uint32_t *)(GPIOB_BASE + OSPEEDR_OFFSET))
// #define GPIOB_PUPDR         (*(volatile uint32_t *)(GPIOB_BASE + PUPDR_OFFSET))
// #define GPIO_PIN_SET(PIN)   (1 << (PIN))
// #define GPIO_PIN_RESET(PIN) (1 << ((PIN) + 16))

// #define GPIOB_OSPEED_PIN0_MID      0x001UL
// #define GPIOB_OSPEED_PIN7_MID      0x004000UL
// #define GPIOB_OSPEED_PIN14_MID     0x0010000000UL 

// #define GPIOB_MODE_PIN0_OUT        0x001UL
// #define GPIOB_MODE_PIN7_OUT        0x004000UL
// #define GPIOB_MODE_PIN14_OUT       0x0010000000UL 
// //按钮1
// #define GPIOC_IDR_PIN13             0x002000UL 
// //按钮2
// #define GPIOC_IDR_PIN12             0x001000UL
// // 引脚定义
// #define LED1_PIN 0   // PB0
// #define LED2_PIN 7   // PB7
// #define LED3_PIN 14  // PB14

// void GPIO_Ini(void);

// //////////////////////////////////////////

// // #include "../../CMSIS/Devices/stm32f4xx.h"
// // #include "../../CMSIS/Devices/stm32f429xx.h"