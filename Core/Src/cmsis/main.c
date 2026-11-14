#include "init.h"

int main(void)
{
    bool output = false;

    GPIO_Ini();

    while (1) {
        bool s1 = READ_BIT(GPIOE->IDR, GPIO_IDR_ID7) == 0U;
        bool s2 = READ_BIT(GPIOE->IDR, GPIO_IDR_ID8) == 0U;

        // S1 выбирает вход, S2 — выход.
        if (s1 && output) {
            CLEAR_BIT(GPIOE->MODER, GPIO_MODER_MODER10);
            CLEAR_BIT(GPIOE->MODER, GPIO_MODER_MODER9);
            output = false;
        } else if (!s1 && s2 && !output) {
            SET_BIT(GPIOE->MODER, GPIO_MODER_MODER9_0);
            SET_BIT(GPIOE->MODER, GPIO_MODER_MODER10_0);
            output = true;
        }

        bool s3 = !output && (READ_BIT(GPIOE->IDR, GPIO_IDR_ID9) == 0U);
        WRITE_REG(GPIOD->BSRR, s1 ? GPIO_BSRR_BS_14 : GPIO_BSRR_BR_14);
        WRITE_REG(GPIOD->BSRR, s2 ? GPIO_BSRR_BS_15 : GPIO_BSRR_BR_15);
        WRITE_REG(GPIOD->BSRR, s3 ? GPIO_BSRR_BS_13 : GPIO_BSRR_BR_13);
    }
}
