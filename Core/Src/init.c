#include "../Inc/init.h"
#include "init.h"

/* =========================================================
 * PORT A — ВРУЧНУЮ ПО АБСОЛЮТНЫМ АДРЕСАМ (без переменных)
 * PA0: Input Pull-Up (кнопка), PA5: Output PP Medium (LED)
 * Управление LED через BSRR
 * =========================================================
 */

void GPIO_Init_PortA_Manual(void)
{
    /* Тактирование GPIOA (бит0) */
    *(uint32_t *)(0x40023800UL + 0x30UL) |= 0x00000001UL;
    /* === PA0: Input + Pull-Up === */
    /* MODER0 = 00 */
    *(uint32_t *)(0x40020000UL + 0x00UL) &= ~0x00000003UL;
    /* PUPDR0 = 01 */
    *(uint32_t *)(0x40020000UL + 0x0CUL) &= ~0x00000003UL;
    *(uint32_t *)(0x40020000UL + 0x0CUL) |= 0x00000001UL;
    /* === PA5: Output, PP, Medium, NoPull === */
    /* MODER5 = 01 */
    *(uint32_t *)(0x40020000UL + 0x00UL) &= ~0x00000C00UL;
    *(uint32_t *)(0x40020000UL + 0x00UL) |= 0x00000400UL;
    /* OTYPER5 = 0 (PP) */
    *(uint32_t *)(0x40020000UL + 0x04UL) &= ~0x00000020UL;
    /* OSPEEDR5 = 01 */
    *(uint32_t *)(0x40020000UL + 0x08UL) &= ~0x00000C00UL;
    *(uint32_t *)(0x40020000UL + 0x08UL) |= 0x00000400UL;
    /* PUPDR5 = 00 */
    *(uint32_t *)(0x40020000UL + 0x0CUL) &= ~0x00000C00UL;
    /* Погасить PA5 через BSRR: BR5 (1<<(5+16)) */
    *(uint32_t *)(0x40020000UL + 0x18UL) = 0x00200000UL;
}

/* ======================================================
 * PORT C — ЧЕРЕЗ ПРОСТЫЕ МАКРОСЫ (кнопка PC13)
 * ======================================================
 */
void GPIO_Init_PortC_Macros(void)
{
    /* Тактирование GPIOC (бит2) */
    BIT_SET(RCC_AHB1ENR_REG, 0x00000004UL);
    /* PC13: Input, без внутренних подтяжек (на плате — pull-down) */
    BIT_CLEAR(GPIOC_MODER_REG, PC13_MODER_MASK); /* MODER13 = 00 */
    BIT_CLEAR(GPIOC_PUPDR_REG, PC13_PUPDR_MASK); /* PUPDR13 = 00 */

    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPD13_1); // PUPDR13 = 10 pull-down
}

/* ======================================================
 * PORT E — ЧЕРЕЗ ПРОСТЫЕ МАКРОСЫ (LED на PE0, BSRR)
 * ======================================================
 */
void GPIO_Init_PortE_Macros(void)
{
    /* Тактирование GPIOE (бит4) */
    BIT_SET(RCC_AHB1ENR_REG, 0x00000010UL);

    /* PE0: Output, PP, Medium, NoPull */
    BIT_CLEAR(GPIOE_MODER_REG, PE0_MODER_CLR);
    BIT_SET(GPIOE_MODER_REG, PE0_MODER_OUT01);

    BIT_CLEAR(GPIOE_OTYPER_REG, PE0_OTYPER_BIT);
    BIT_CLEAR(GPIOE_OSPEEDR_REG, PE0_OSPEEDR_CLR);

    BIT_SET(GPIOE_OSPEEDR_REG, PE0_OSPEEDR_MED01);
    BIT_CLEAR(GPIOE_PUPDR_REG, PE0_PUPDR_CLR);

    /* Погасить PE0 через BSRR */
    GPIOE_BSRR_REG = PE0_BSRR_RESET;
}

/* =========================================
 * PORT D — PD12 (LED) — ЧЕРЕЗ CMSIS + BSRR
 * =========================================
 */
void GPIO_Init_PortD_CMSIS(void)
{
    /* Тактирование GPIOD */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);

    /* PD12: Output, PP, Medium, NoPull */
    SET_BIT(GPIOD->MODER, GPIO_MODER_MODE12_0);         /* 01 */
    CLEAR_BIT(GPIOD->OTYPER, GPIO_OTYPER_OT12);         /* PP */
    SET_BIT(GPIOD->OSPEEDR, GPIO_OSPEEDER_OSPEEDR12_0); /* 01 */
    CLEAR_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPD12);         /* 00 */

    /* Погасить PD12 через BSRR */
    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR12);
}
