#include "init.h"

/*
 * Variant 8: 6 LEDs, 3 buttons
 *   LED1 - PB0  (green, board)  - direct memory access
 *   LED2 - PB7  (blue, board)   - custom defines (init.h)
 *   LED3 - PB14 (red, board)    - CMSIS
 *   LED4 - PA5  (D13, external) - CMSIS
 *   LED5 - PA6  (D12, external) - CMSIS
 *   LED6 - PA7  (D11, external) - CMSIS
 *   Button 1 - PC13 (board B1)
 *   Button 2 - PA3  (A0, HW-483)
 *   Button 3 - PC0  (A1)
 */

void GPIO_Ini(void)
{
    /* Clocks for GPIOA, GPIOB, GPIOC */
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);
    (void)READ_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

    /* LED1: PB0, direct memory access */
    *(volatile uint32_t*)(0x40020400UL + 0x00UL) |= 0x00000001UL;
    *(volatile uint32_t*)(0x40020400UL + 0x04UL) &= 0xFFFFFFFEUL;
    *(volatile uint32_t*)(0x40020400UL + 0x08UL) |= 0x00000001UL;
    *(volatile uint32_t*)(0x40020400UL + 0x0CUL) &= 0xFFFFFFFCUL;

    /* LED2: PB7, custom defines */
    MY_SET_BITS(MY_GPIOB_MODER,   MY_PIN7_MODE_OUT);
    MY_CLR_BITS(MY_GPIOB_OTYPER,  MY_PIN7_OTYPE_PP_MASK);
    MY_SET_BITS(MY_GPIOB_OSPEEDR, MY_PIN7_OSPEED_MID);
    MY_CLR_BITS(MY_GPIOB_PUPDR,   MY_PIN7_PUPDR_NONE_MASK);

    /* LED3: PB14, CMSIS */
    SET_BIT(GPIOB->MODER,    GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT_14);
    SET_BIT(GPIOB->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR14_0);
    CLEAR_BIT(GPIOB->PUPDR,  GPIO_PUPDR_PUPDR14);

    /* LED4: PA5 (D13), CMSIS */
    SET_BIT(GPIOA->MODER,    GPIO_MODER_MODE5_0);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_5);
    SET_BIT(GPIOA->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR5_0);
    CLEAR_BIT(GPIOA->PUPDR,  GPIO_PUPDR_PUPDR5);

    /* LED5: PA6 (D12), CMSIS */
    SET_BIT(GPIOA->MODER,    GPIO_MODER_MODE6_0);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_6);
    SET_BIT(GPIOA->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR6_0);
    CLEAR_BIT(GPIOA->PUPDR,  GPIO_PUPDR_PUPDR6);

    /* LED6: PA7 (D11), CMSIS */
    SET_BIT(GPIOA->MODER,    GPIO_MODER_MODE7_0);
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT_7);
    SET_BIT(GPIOA->OSPEEDR,  GPIO_OSPEEDER_OSPEEDR7_0);
    CLEAR_BIT(GPIOA->PUPDR,  GPIO_PUPDR_PUPDR7);

    /* Button 1: PC13, input (pull-down is on the board) */

    /* Button 2: PA3 (A0), input, pull-down */
    SET_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPDR3_1);

    /* Button 3: PC0 (A1), input, pull-down */
    SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR0_1);

    Leds_Set(0);
}

/* Turn on the first n LEDs, turn off the rest */
void Leds_Set(uint8_t n)
{
    /* LED1 - PB0, direct memory access */
    if (n >= 1) { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x00000001UL; }
    else        { *(volatile uint32_t*)(0x40020400UL + 0x18UL) = 0x00010000UL; }

    /* LED2 - PB7, custom defines */
    if (n >= 2) { MY_GPIOB_BSRR = MY_PIN7_SET; }
    else        { MY_GPIOB_BSRR = MY_PIN7_RESET; }

    /* LED3 - PB14, CMSIS */
    if (n >= 3) { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BS14); }
    else        { WRITE_REG(GPIOB->BSRR, GPIO_BSRR_BR14); }

    /* LED4 - PA5, CMSIS */
    if (n >= 4) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS5); }
    else        { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR5); }

    /* LED5 - PA6, CMSIS */
    if (n >= 5) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS6); }
    else        { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR6); }

    /* LED6 - PA7, CMSIS */
    if (n >= 6) { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BS7); }
    else        { WRITE_REG(GPIOA->BSRR, GPIO_BSRR_BR7); }
}

/* Raw level of button n (0..2), CMSIS */
uint8_t Btn_Raw(uint8_t n)
{
    if (n == 0) { return (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13) != 0) ? 1 : 0; }
    if (n == 1) { return (READ_BIT(GPIOA->IDR, GPIO_IDR_IDR_3)  != 0) ? 1 : 0; }
    return (READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_0) != 0) ? 1 : 0;
}