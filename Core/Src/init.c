#include "init.h"

void GPIO_Init(void) {
    RCC_AHB1ENR |= (1UL << 6) | (1UL << 5) | (1UL << 4) | (1UL << 3);

    GPIOG_MODER &= ~((3UL << (9 * 2)) | (3UL << (14 * 2)));
    GPIOG_MODER |= ((1UL << (9 * 2)) | (1UL << (14 * 2)));
    GPIOF_MODER &= ~((3UL << (15 * 2)) | (3UL << (14 * 2)));
    GPIOF_MODER |= ((1UL << (15 * 2)) | (1UL << (14 * 2)));
    GPIOE_MODER &= ~((3UL << (13 * 2)) | (3UL << (11 * 2)));
    GPIOE_MODER |= ((1UL << (13 * 2)) | (1UL << (11 * 2)));

    GPIOF_MODER &= ~(3UL << (12 * 2));
    GPIOD_MODER &= ~(3UL << (15 * 2));
    GPIOF_PUPDR |= (2UL << (12 * 2));
    GPIOD_PUPDR |= (2UL << (15 * 2));
}
