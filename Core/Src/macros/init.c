#include "init.h"

// Вызывается перед main. Оставляем тактирование после сброса.
void SystemInit(void)
{
}

/* PE7/PE8: active-low mode buttons. PE9: shared line. PE10: LED supply. */
void GPIO_Ini(void)
{
    SET_BIT(RCC_AHB1ENR, CLOCKS);
    (void)RCC_AHB1ENR;

    CLEAR_BIT(GPIOE_MODER, E_MODES);
    GPIOE_OTYPER = (GPIOE_OTYPER & ~E_PUSH_PULL) | SHARED_OPEN_DRAIN;
    GPIOE_OSPEEDR &= ~E_MODES;
    GPIOE_PUPDR = (GPIOE_PUPDR & ~E_MODES) | BUTTON_PULLUPS;
    /* Preload PE9 low and PE10 high while both pins are inputs. */
    WRITE_REG(GPIOE_BSRR, SHARED_LOW | LED_SUPPLY_HIGH);

    GPIOD_OTYPER &= ~D_PINS;
    GPIOD_OSPEEDR &= ~D_MODES;
    GPIOD_PUPDR &= ~D_MODES;
    WRITE_REG(GPIOD_BSRR, RED_OFF | BLUE_OFF | ORANGE_OFF);
    GPIOD_MODER = (GPIOD_MODER & ~D_MODES) | D_OUTPUTS;
}

void GPIO_SetSharedOutput(bool output)
{
    if (output) {
        GPIOE_MODER = (GPIOE_MODER & ~SHARED_MODE) | SHARED_OUTPUT;
        GPIOE_MODER = (GPIOE_MODER & ~LED_SUPPLY_MODE) | LED_SUPPLY_OUTPUT;
    } else {
        GPIOE_MODER &= ~LED_SUPPLY_MODE;
        GPIOE_MODER &= ~SHARED_MODE;
    }
}

bool GPIO_ReadInputButton(void)
{
    return READ_BIT(GPIOE_IDR, INPUT_BUTTON) == 0U;
}

bool GPIO_ReadOutputButton(void)
{
    return READ_BIT(GPIOE_IDR, OUTPUT_BUTTON) == 0U;
}

bool GPIO_ReadSharedButton(void)
{
    return READ_BIT(GPIOE_IDR, SHARED_BUTTON) == 0U;
}

void GPIO_SetIndicators(bool red, bool blue, bool orange)
{
    WRITE_REG(GPIOD_BSRR, (red ? RED_ON : RED_OFF) |
                         (blue ? BLUE_ON : BLUE_OFF) |
                         (orange ? ORANGE_ON : ORANGE_OFF));
}
