#include "C:\Users\mns94\OneDrive\Documents\digital_microprocessor_technology\Laboratory_Practice-main\CMSIS\Devices\stm32f4xx.h"
#include "C:\Users\mns94\OneDrive\Documents\digital_microprocessor_technology\Laboratory_Practice-main\CMSIS\Devices\stm32f429xx.h"

#define GPIOB_MODER (*(uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_OTYPER (*(uint32_t*)(0x40020400UL + 0x04UL)) 
#define GPIOB_OSPEEDR (*(uint32_t*)(0x40020400UL + 0x08UL)) 
#define GPIOB_PUPDR (*(uint32_t*)(0x40020400UL + 0x0CUL))
#define GPIOB_BSRR (*(uint32_t*)(0x40020400UL + 0x18UL))

#define GPIOB_MODE_PIN0_OUT 0x0001UL 
#define GPIOB_OTYPE_PIN0_PP 0x0000UL 
#define GPIOB_OSPEED_PIN0_MID 0x0001UL 
#define GPIOB_PUPDR_PIN0_NOPUPD 0x0000UL
#define GPIOB_BSRR_PIN0_SET 0x000001UL 
#define GPIOB_BSRR_PIN0_RESET 0x010000UL

void GPIO_Ini(void);
void GPIO_Ini_Self_Def(void);
void GPIO_Ini_CMSIS(void);


//#include <stdint.h>

// #define RCC_GPIO_EN     (*(uint32_t*)(0x40023800UL + 0x30UL))
// #define RCC_GPIOB_EN    0X02UL
// #define RCC_GPIOC_EN    0X04UL

// #define GPIOB_MODER     (*(uint32_t*)(0x40020400UL + 0x00UL))
// #define GPIOB_OTYPER    (*(uint32_t*)(0x40020400UL + 0x04UL))
// #define GPIOB_OSPEEDR   (*(uint32_t*)(0x40020400UL + 0x08UL))
// #define GPIOB_PUPDR     (*(uint32_t*)(0x40020400UL + 0x0CUL))
// #define GPIOB_BSRR      (*(uint32_t*)(0x40020400UL + 0x18UL))

// #define GPIOC_IDR       (*(uint32_t*)(0x40020800UL + 0x10UL))
// #define GPIOC_IDR_PIN13 0x002000UL

// #define GPIOB_MODE_PIN7_OUT     0x004000UL
// #define GPIOB_OTYPE_PIN7_PP     0x000000UL
// #define GPIOB_OSPEED_PIN7_MID   0x004000UL
// #define GPIOB_PUPDR_PIN7_NOPUPD 0x000000UL
// #define GPIOB_BSRR_PIN7_SET     0x000080UL
// #define GPIOB_BSTT_PIN7_RESET   0x800000UL

// #define READ_BIT(REG,BIT)       (REG & BIT)
// #define SET_BIT(REG, BIT)       (REG |= BIT)