#include "init.h"

// Вызывается перед main. Оставляем тактирование после сброса.
void SystemInit(void)
{
}

/* PE7/PE8: active-low mode buttons. PE9: shared line. PE10: LED supply. */
void GPIO_Ini(void)
{
    /* RCC_AHB1ENR: enable the GPIOD and GPIOE clocks. */
    (*(volatile uint32_t *)0x40023830UL) |= ((1UL << 3) | (1UL << 4));
    (void)(*(volatile uint32_t *)0x40023830UL);

    /* GPIOE: MODER, OTYPER, OSPEEDR, PUPDR and BSRR. */
    (*(volatile uint32_t *)0x40021000UL) &= ~((3UL << 14) | (3UL << 16) | (3UL << 18) | (3UL << 20));
    (*(volatile uint32_t *)0x40021004UL) =
        ((*(volatile uint32_t *)0x40021004UL) & ~((1UL << 7) | (1UL << 8) | (1UL << 10))) |
        (1UL << 9);
    (*(volatile uint32_t *)0x40021008UL) &= ~((3UL << 14) | (3UL << 16) | (3UL << 18) | (3UL << 20));
    (*(volatile uint32_t *)0x4002100CUL) = ((*(volatile uint32_t *)0x4002100CUL) & ~((3UL << 14) | (3UL << 16) | (3UL << 18) | (3UL << 20))) | ((1UL << 14) | (1UL << 16) | (1UL << 18));
    /* Preload PE9 low and PE10 high while both pins are inputs. */
    (*(volatile uint32_t *)0x40021018UL) = (1UL << 25) | (1UL << 10);

    /* GPIOD: configure the three on-board LEDs. */
    (*(volatile uint32_t *)0x40020C04UL) &= ~((1UL << 13) | (1UL << 14) | (1UL << 15));
    (*(volatile uint32_t *)0x40020C08UL) &= ~((3UL << 26) | (3UL << 28) | (3UL << 30));
    (*(volatile uint32_t *)0x40020C0CUL) &= ~((3UL << 26) | (3UL << 28) | (3UL << 30));
    (*(volatile uint32_t *)0x40020C18UL) = (1UL << 30) | (1UL << 31) | (1UL << 29);
    (*(volatile uint32_t *)0x40020C00UL) = ((*(volatile uint32_t *)0x40020C00UL) & ~((3UL << 26) | (3UL << 28) | (3UL << 30))) | ((1UL << 26) | (1UL << 28) | (1UL << 30));
}

void GPIO_SetSharedOutput(bool output)
{
    if (output) {
        (*(volatile uint32_t *)0x40021000UL) = ((*(volatile uint32_t *)0x40021000UL) & ~(3UL << 18)) | (1UL << 18);
        (*(volatile uint32_t *)0x40021000UL) = ((*(volatile uint32_t *)0x40021000UL) & ~(3UL << 20)) | (1UL << 20);
    } else {
        (*(volatile uint32_t *)0x40021000UL) &= ~(3UL << 20);
        (*(volatile uint32_t *)0x40021000UL) &= ~(3UL << 18);
    }
}

bool GPIO_ReadInputButton(void)
{
    return ((*(volatile uint32_t *)0x40021010UL) & (1UL << 7)) == 0U;
}

bool GPIO_ReadOutputButton(void)
{
    return ((*(volatile uint32_t *)0x40021010UL) & (1UL << 8)) == 0U;
}

bool GPIO_ReadSharedButton(void)
{
    return ((*(volatile uint32_t *)0x40021010UL) & (1UL << 9)) == 0U;
}

void GPIO_SetIndicators(bool red, bool blue, bool orange)
{
    (*(volatile uint32_t *)0x40020C18UL) = (red ? (1UL << 14) : (1UL << 30)) |
                 (blue ? (1UL << 15) : (1UL << 31)) |
                 (orange ? (1UL << 13) : (1UL << 29));
}
