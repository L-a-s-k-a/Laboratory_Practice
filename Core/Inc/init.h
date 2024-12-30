#ifndef INIT_H
#define INIT_H

#include  "../../CMSIS/Devices/stm32f4xx.h"
#include  "../../CMSIS/Devices/stm32f429xx.h"

// 开启时钟B, C
#define RCC_GPIO_EN ((uint32_t)(0x40023800UL + 0x30UL))
#define RCC_GPIOB_EN 0x02UL // 时钟B
#define RCC_GPIOC_EN 0x04UL // 时钟C

// GPIOB 和 GPIOC 寄存器定义
#define GPIOB_MODER ((uint32_t)(0x40020400UL + 0x00UL))
#define GPIOB_OTYPER ((uint32_t)(0x40020400UL + 0x04UL))
#define GPIOB_OSPEEDR ((uint32_t)(0x40020400UL + 0x08UL))
#define GPIOB_PUPDR ((uint32_t)(0x40020400UL + 0x0CUL))
#define GPIOB_BSRR ((uint32_t)(0x40020400UL + 0x18UL))
#define GPIOC_IDR ((uint32_t)(0x40020800UL + 0x10UL))

// 按钮定义
#define GPIOC_IDR_PIN13 0x002000UL

// LED1 (GREEN)
#define GPIOB_MODE_PIN0_OUT 0x001UL
#define GPIOB_BSRR_PIN0_SET 0x001UL
#define GPIOB_BSRR_PIN0_RESET 0x010000UL

// LED2 (BLUE)
#define GPIOB_MODE_PIN7_OUT 0x004000UL
#define GPIOB_BSRR_PIN7_SET 0x000080UL
#define GPIOB_BSRR_PIN7_RESET 0x800000UL

// LED3 (RED)
#define GPIOB_MODE_PIN14_OUT 0x10000000UL
#define GPIOB_BSRR_PIN14_SET 0x00004000UL
#define GPIOB_BSRR_PIN14_RESET 0x40000000UL

// LED4 (外接LED)
#define GPIOB_MODE_PIN10_OUT 0x100000UL
#define GPIOB_BSRR_PIN10_SET 0x000400UL
#define GPIOB_BSRR_PIN10_RESET 0x4000000UL

// 辅助宏
#define READ_BIT(REG, BIT) ((REG) & (BIT))
#define SET_BIT(REG, BIT) ((REG) |= (BIT))

// 函数声明
void GPIO_Ini(void);
void ToggleLED(uint8_t led_index, uint8_t state);
void TurnOffAllLEDs(void);
void TurnOnAllLEDs(void);

#endif // INIT_H
