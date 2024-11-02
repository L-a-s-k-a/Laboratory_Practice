#include "../Inc/init.h"

uint8_t flag1, flag1_pred;
int button_count = 0, button_drebezg, button;
int main(void)

{
    GPIO_Ini(); // инициализация портов
    while (1)
    {
        button = READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13);
        // -------------------------------------------------
        if (button != 0) // проверка бита
        {
            flag1 = 1;
        }
        if (flag1 == 1)
        {
            button_drebezg++;
        }
        if (button_drebezg >= 100000)
        {
            if (button != 0)
            {
                if (flag1_pred == 0)
                {
                    button_count++; /* code */
                    flag1_pred = 1;
                    button_drebezg = 0;
                }
            }
            else
            {
                flag1 = 0;
                flag1_pred = 0;
                button_drebezg = 0;
            }
        }

        if (flag1 == 1 && flag1_pred == 0)
        {
            button_count++;
        }
        flag1_pred = flag1;
        //-------------------------------------------------
        // lab1
        switch (button_count)
        {
        case 1:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 2:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 3:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS12); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 4:
            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN6_SET); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 5:
            // сбросить все лампы (_БР)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR12);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR6);
            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN6_RESET);
            break;
        case 6:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS12);
            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN6_SET); 
            break;
        default:
            // зажечь все лампы (_БС)
            button_count = 0;
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR12);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR6);
            SET_BIT(GPIOB_BSRR, GPIOB_BSRR_PIN6_RESET);
            break;
        }
    }
}