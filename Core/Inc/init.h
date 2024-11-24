#include <stdint.h>


#include"../../"

// 开启时钟B,C
#define RCC_GPIO_EN                   (*(uint32_t*)(0x40023800UL + 0x30UL))
#define RCC_GPIOB_EN                  0x02UL // 时钟B
#define RCC_GPIOC_EN                  0x04UL // 时钟C

// GPIOB 寄存器
#define GPIOB_MODER                   (*(uint32_t*)(0x40020400UL + 0x00UL))
#define GPIOB_OTYPER                  (*(uint32_t*)(0x40020400UL + 0x04UL))
#define GPIOB_OSPEEDR                 (*(uint32_t*)(0x40020400UL + 0x08UL))
#define GPIOB_PUPDR                   (*(uint32_t*)(0x40020400UL + 0x0CUL)) 
#define GPIOB_BSRR                    (*(uint32_t*)(0x40020400UL + 0x18UL))

// GPIOC 寄存器
#define GPIOC_IDR                     (*(uint32_t*)(0x40020800UL + 0x10UL))

// 按钮引脚
#define GPIOC_IDR_PIN13               0x002000UL

// LED1 (绿色)
#define GPIOB_MODE_PIN0_OUT           (0x01 << (0 * 2)) // MODER
#define GPIOB_BSRR_PIN0_SET           (0x01) // 点亮
#define GPIOB_BSRR_PIN0_RESET         (0x010000) // 熄灭

// LED2 (蓝色)
#define GPIOB_MODE_PIN7_OUT           (0x01 << (7 * 2)) // MODER
#define GPIOB_BSRR_PIN7_SET           (0x80) // 点亮
#define GPIOB_BSRR_PIN7_RESET         (0x800000) // 熄灭

// LED3 (红色)
#define GPIOB_MODE_PIN14_OUT          (0x01 << (14 * 2)) // MODER
#define GPIOB_BSRR_PIN14_SET          (0x4000) // 点亮
#define GPIOB_BSRR_PIN14_RESET        (0x40000000) // 熄灭

// LED4 (外接LED)
#define GPIOB_MODE_PIN10_OUT          (0x01 << (10 * 2)) // MODER
#define GPIOB_BSRR_PIN10_SET          (0x400) // 点亮
#define GPIOB_BSRR_PIN10_RESET        (0x400000) // 熄灭

#define READ_BIT(REG, BIT) (REG & BIT)
#define SET_BIT(REG, BIT) (REG |= BIT)

void GPIO_Ini(void);