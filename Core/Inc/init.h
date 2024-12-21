#include  "../../CMSIS\CMSIS\Devices\Inc/stm32f4xx.h"
#include  "../../CMSIS\\CMSIS\Devices\Inc/stm32f429xx.h"
//开启时钟 B,C
#define RCC_GPIO_EN                 (*(uint32_t*)(0x40023800UL + 0x30UL)) 
#define RCC_GPIOB_EN                0x02UL 
#define RCC_GPIOC_EN                0x04UL 
 //设置时钟
#define GPIOB_MODER                 (*(uint32_t*)(0x40020400UL + 0x00UL)) 
#define GPIOB_OTYPER                (*(uint32_t*)(0x40020400UL + 0x04UL)) 
#define GPIOB_OSPEEDR               (*(uint32_t*)(0x40020400UL + 0x08UL)) 
#define GPIOB_PUPDR                 (*(uint32_t*)(0x40020400UL + 0x0CUL))  
#define GPIOB_BSRR                  (*(uint32_t*)(0x40020400UL + 0x18UL)) 
 //设置按钮
#define GPIOC_IDR                   (*(uint32_t*)(0x40020800UL + 0x10UL)) 
//按钮1
#define GPIOC_IDR_PIN13             0x002000UL 
//按钮2
#define GPIOC_IDR_PIN12             0x001000UL  
//设置LED1
#define GPIOB_MODE_PIN0_OUT        0x001UL 
#define GPIOB_OTYPE_PIN0_PP        0x000000UL 
#define GPIOB_OSPEED_PIN0_MID      0x001UL 
#define GPIOB_PUPDR_PIN0_NOPUPD    0x000000UL
#define GPIOB_BSRR_PIN0_SET        0x001UL 
#define GPIOB_BSRR_PIN0_RESET      0x0010000UL
//设置LED2
#define GPIOB_MODE_PIN7_OUT         0x004000UL       
#define GPIOB_OTYPE_PIN7_PP         0x000000UL 
#define GPIOB_OSPEED_PIN7_MID       0x004000UL 
#define GPIOB_PUPDR_PIN7_NOPUPD     0x000000UL 
#define GPIOB_BSRR_PIN7_SET         0x000080UL 
#define GPIOB_BSRR_PIN7_RESET       0x800000UL 
//设置LED3
#define GPIOB_MODE_PIN14_OUT        0x0010000000UL 
#define GPIOB_OTYPE_PIN14_PP        0x000000UL 
#define GPIOB_OSPEED_PIN14_MID      0x0010000000UL 
#define GPIOB_PUPDR_PIN14_NOPUPD    0x000000UL
#define GPIOB_BSRR_PIN14_SET        0x004000UL 
#define GPIOB_BSRR_PIN14_RESET      0x0040000000UL
//设置LED4
#define GPIOB_MODE_PIN8_OUT        0x0010000UL 
#define GPIOB_OTYPE_PIN8_PP        0x000000UL 
#define GPIOB_OSPEED_PIN8_MID      0x0010000UL 
#define GPIOB_PUPDR_PIN8_NOPUPD    0x000000UL
#define GPIOB_BSRR_PIN8_SET        0x00100UL 
#define GPIOB_BSRR_PIN8_RESET      0x001000000UL

 
#define READ_BIT(REG, BIT)          (REG & BIT) 
#define SET_BIT(REG, BIT)           (REG |= BIT) 
void GPIO_Ini_1(void);
void GPIO_Ini_2(void);
