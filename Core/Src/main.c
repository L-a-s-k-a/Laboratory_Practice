#include "../Inc/init.h"

uint8_t flag1, flag1_pred;
int button_count = 0, button_drebezg;
int main(void)

{
    GPIO_Ini(); // инициализация портов
    while (1)
    {
        int button = READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13);
        // -------------------------------------------------
        if (button == 0) // проверка бита
        {
            if (button_drebezg < 10)
            {
                button_drebezg++;
            }
            if (button_drebezg >= 10)
            {
                //   SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // Установка единицы в 7-ой бит регистра ODR
                flag1 = 1;
            }
        }
        else
        {
            //  button_drebezg--;
            if (button_drebezg > 0)
            {
                button_drebezg--;
            }
            if (button_drebezg <= 0)
            {
                //   SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); // Установка нуля в 7-ой бит регистра ODR
                flag1 = 0;
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
        case 0:
            break;
        case 1:
            break;
        case 2:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13); // Установка единицы в 7-ой бит регистра ODR
            //    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 3:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 4:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS12); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 5:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 6:
            // сбросить все лампы (_БР)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR13);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR15);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR12);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR8);
            break;
        case 7:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS12);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8);
            break;
        default:
            // зажечь все лампы (_БС)
            button_count = 0;
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS13);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS15);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS12);
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS8);
            break;
        }
    }
}