#include <stdint.h>
#include "../../CMSIS/Devices/system_stm32f4xx.h"
#include "../../CMSIS/Devices/stm32f429xx.h"
#include "../../CMSIS/Devices/stm32f4xx.h"


// #define RCC_GPIO_EN (*(uint32_t*)(0x40023800UL + 0x30UL)) // Тактирование

// #define RCC_GPIOB_EN 0x02UL
// #define RCC_GPIOC_EN 0x04UL

// #define GPIOB_MODER (*(uint32_t*)(0x40020400UL + 0x00UL)) // Ввод-вывод
// #define GPIOB_OTYPER (*(uint32_t*)(0x40020400UL + 0x04UL)) // PUSH/PULL
// #define GPIOB_OSPEEDER (*(uint32_t*)(0x40020400UL + 0x08UL)) //скорость работы
// #define GPIOB_PUPDR (*(uint32_t*)(0x40020400UL + 0x0CUL)) // 
// #define GPIOB_BSRR (*(uint32_t*)(0x40020400UL + 0x18UL))

// #define GPIOC_IDR  (*(uint32_t*)(0x40020800UL + 0x10UL))

// #define GPIOB_MODE_PIN7_OUT 0x4000UL
// #define GPIOB_OTYPE_PIN7_PP 0x0000UL
// #define GPIOB_OSPEED_PIN7_MID 0x4000UL
// #define GPIOB_PUPDR_PIN7_NOPUPD 0x0000UL
// #define GPIOB_BSRR_PIN7_SET 0x000080UL
// #define GPIOB_BSRR_PIN7_RESET 0x800000UL

// #define GPIOC_IDR_PIN13 0x002000UL

// #define READ_BIT(REG, BIT) (REG & BIT)
// #define SET_BIT(REG, BIT) (REG |= BIT)

// #define READ_GPIO_C13 (*(uint32_t*)(0x40020800UL + 0x10UL) & 0x2000UL)
// #define SET_GPIO_B7 (*(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80)
// #define RESET_GPIO_B7 (*(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000)

void GPIO_Init(void);
void GPIO_led_init(void);
void GPIO_button_output(void);
void GPIO_button_input(void);
void button1_output(void);
void button2_output(void);
void button3_output(void);
void button1_input(void);
void button2_input(void);
void button3_input(void);
// #define GPIOB_RCC_ON *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x02; // включение тактирования GPIOB
// #define GPIOC_RCC_ON *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x04; // включение тактирования GPIOC 
// #define PIN7_OUT *(uint32_t *)(0x40020400UL + 0x00UL) |= 0x4000;   // 7пин на вывод
// #define PIN7_PUSH_PULL *(uint32_t *)(0x40020400UL + 0x04UL) |= 0x00;     // 7пин push-pull
// #define PIN7_SPEED *(uint32_t *)(0x40020400UL + 0x08UL) |= 0x4000;   // 7пин средняя скорость

// #define READ_PIN_C13 ((*(uint32_t *)(0x40020800UL + 0x10UL) & 0x2000))
// #define PIN7_SET *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x80; // 7пин - 1 (0DR)
// #define PIN7_RESET *(uint32_t *)(0x40020400UL + 0x18UL) |= 0x800000; // 7пин - 0 (0DR)


// #define RCC_GPIOB_EN 0x02UL
// #define RCC_GPIOC_EN 0x04UL

// #define GPIOB_MODE_PIN7_OUT 0x4000UL
// #define GPIOB_OTYPE_PIN7_PP 0x0000UL
// #define GPIOB_OSSPEED_PIN7_MID 0x4000UL
// #define GPIOB_PUPDR_PIN7_NOPUPD 0x0000UL

// void GPIO_Ini(void);