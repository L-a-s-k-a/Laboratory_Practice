#include "init.h"

void GPIO_Ini(void)
{
    RCC_GPIO_EN   |= RCC_GPIOB_EN + RCC_GPIOC_EN; // Включение тактирования GPIOB и GPIOC

    GPIOB_MODER   |= GPIOB_MODE_LEDS_OUT;         // Режим вывода (Output mode)
    GPIOB_OTYPER  |= GPIOB_OTYPE_LEDS_PP;         // Режим Push-Pull
    GPIOB_OSPEEDR |= GPIOB_OSPEED_LEDS_MID;       // Средняя скорость
    GPIOB_PUPDR   |= GPIOB_PUPDR_LEDS_NOPUPD;     // Без подтяжки

    GPIOC_PUPDR   |= GPIOC_PUPDR_BTNS_PD;         // Стяжка к земле для PC0 и PC3
}