#include <stdint.h>
//#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F429ZI/stm32f429xx.h"
#include "stm32f4xx.h"

/* -------- простые макросы -------- */
#define BIT_SET(REG, BITMASK) ((REG) |= (uint32_t)(BITMASK))
#define BIT_CLEAR(REG, BITMASK) ((REG) &= ~(uint32_t)(BITMASK))
#define BIT_READ(REG, BITMASK) ((REG) & (uint32_t)(BITMASK))

/* -------- RCC -------- */
#define RCC_AHB1ENR_REG (*(volatile uint32_t *)(0x40023800UL + 0x30UL))

/* -------- GPIOC (кнопка PC13) -------- */
#define GPIOC_MODER_REG (*(volatile uint32_t *)(0x40020800UL + 0x00UL))
#define GPIOC_PUPDR_REG (*(volatile uint32_t *)(0x40020800UL + 0x0CUL))
#define GPIOC_IDR_REG (*(volatile uint32_t *)(0x40020800UL + 0x10UL))

/* PC13 поля */
#define PC13_MODER_MASK 0x0C000000UL /* MODER13 (27:26) */
#define PC13_PUPDR_MASK 0x0C000000UL /* PUPDR13 (27:26) */
#define PC13_IDR_BIT 0x00002000UL    /* IDR13 */

/* -------- GPIOE (LED на PE0) -------- */
#define GPIOE_MODER_REG (*(volatile uint32_t *)(0x40021000UL + 0x00UL))
#define GPIOE_OTYPER_REG (*(volatile uint32_t *)(0x40021000UL + 0x04UL))
#define GPIOE_OSPEEDR_REG (*(volatile uint32_t *)(0x40021000UL + 0x08UL))
#define GPIOE_PUPDR_REG (*(volatile uint32_t *)(0x40021000UL + 0x0CUL))
#define GPIOE_BSRR_REG (*(volatile uint32_t *)(0x40021000UL + 0x18UL))

/* PE0 поля */
#define PE0_MODER_CLR 0x00000003UL     /* MODER0 clear */
#define PE0_MODER_OUT01 0x00000001UL   /* MODER0=01 */
#define PE0_OTYPER_BIT 0x00000001UL    /* OT0 */
#define PE0_OSPEEDR_CLR 0x00000003UL   /* OSPEED0 clear */
#define PE0_OSPEEDR_MED01 0x00000001UL /* OSPEED0=01 (Medium) */
#define PE0_PUPDR_CLR 0x00000003UL     /* PUPDR0=00 */
#define PE0_BSRR_SET 0x00000001UL      /* BS0 */
#define PE0_BSRR_RESET 0x00010000UL    /* BR0 */

/* --- Инициализация портов --- */
/* PA: PA0 (Input Pull-Up), PA5 (LED) — вручную по адресам (без локальных переменных) */
void GPIO_Init_PortA_Manual(void);

/* PC: PC13 (Input, no pull) — через простые макросы */
void GPIO_Init_PortC_Macros(void);

/* PE: PE0 (LED) — через простые макросы */
void GPIO_Init_PortE_Macros(void);

/* PD: PD12 (LED) — через CMSIS */
void GPIO_Init_PortD_CMSIS(void);