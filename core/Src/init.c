
#include "init.h"

void InitializeGPIOSelf(void)
{
    /* Enable GPIOB and GPIOC ports on AHB1 bus */
    SET_BIT_SELF(RCC_AHB1ENR, RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN);

    /* Configure PB7 as output (for LED use) */
    SET_BIT_SELF(GPIOB_MODER, GPIO_PIN_7_OUT);
    SET_BIT_SELF(GPIOB_OSPEEDR, GPIO_PIN_7_MED);
    SET_BIT_SELF(GPIOB_BSRR, GPIO_PIN_7_RESET);

    /* Configure PC13 and PC6 as input */
    GPIOC->MODER &= ~(GPIO_MODER_MODE13);
    /* 
       Could configure as pull-up if needed:
       GPIOC->PUPDR |= (GPIO_PUPDR_PUPD13_0 | GPIO_PUPDR_PUPD6_0);
    */
}

void InitializeGPIOCMSIS(void)
{
    /*********************************************************************
     * Configure PB14 for a red LED
     *********************************************************************/
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE14_0);
    CLEAR_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT14);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR14_0);
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR14);

    /*********************************************************************
     * Configure PB0 for a green LED
     *********************************************************************/
    SET_BIT(GPIOB->MODER, GPIO_MODER_MODE0_0);
    SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0);
    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR0);
}
