#include "init.h"

uint8_t led_count = 0;
uint8_t swap_mode = 0;

int main(void)
{
    GPIO_Ini(); // Инициализация портов GPIO

    uint8_t btn1_flag = 0;
    uint8_t btn2_flag = 0;
    uint8_t btn3_flag = 0;

    uint32_t btn2_start = READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN0);
    uint32_t btn3_start = READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN3);

    while (1)
    {
        // кнопка 3
        if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN3) != btn3_start)
        {
            if (btn3_flag == 0)
            {
                if (swap_mode == 0) swap_mode = 1;
                else swap_mode = 0;
                btn3_flag = 1;
            }
        }
        else
        {
            btn3_flag = 0;
        }

        // кнопка 1
        if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN13) != 0)
        {
            if (btn1_flag == 0)
            {
                if (swap_mode == 0)
                {
                    if (led_count < 6) led_count = led_count + 1;
                }
                else
                {
                    if (led_count > 0) led_count = led_count - 1;
                }
                btn1_flag = 1;
            }
        }
        else
        {
            btn1_flag = 0;
        }

        // кнопка 2
        if (READ_BIT(GPIOC_IDR, GPIOC_IDR_PIN0) != btn2_start)
        {
            if (btn2_flag == 0)
            {
                if (swap_mode == 0)
                {
                    if (led_count > 0) led_count = led_count - 1;
                }
                else
                {
                    if (led_count < 6) led_count = led_count + 1;
                }
                btn2_flag = 1;
            }
        }
        else
        {
            btn2_flag = 0;
        }

        // Управление 6 светодиодами 
        if (led_count >= 1) SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_SET);  else SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN0_RESET);
        if (led_count >= 2) SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_SET);  else SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN7_RESET);
        if (led_count >= 3) SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_SET); else SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN14_RESET);
        if (led_count >= 4) SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN1_SET);  else SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN1_RESET);
        if (led_count >= 5) SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN2_SET);  else SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN2_RESET);
        if (led_count >= 6) SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN15_SET); else SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN15_RESET);

        for (volatile int i = 0; i < 30000; i++);
    }
}