#include "../Inc/init.h"

uint8_t flag1, flag1_pred;
int button_count, button_drebezg;
int main(void)
{
    GPIO_Ini(); // инициализация портов
    while (1)
    {
        int button = READ_BIT(GPIOC->IDR, GPIO_IDR_IDR_13);
        // -------------------------------------------------
        if (button == 1) // проверка бита
        {
            if (button_drebezg < 10)
            {
                button_drebezg++;
            }
            if (button_drebezg >= 10)
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // Установка единицы в 7-ой бит регистра ODR
                flag1 = 1;
            }
        }
        else
        {
            if (button_drebezg > 0)
            {
                button_drebezg--;
            }
            if (button_drebezg <= 0)
            {
                SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); // Установка нуля в 7-ой бит регистра ODR
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
        /*
        if (button == 0)
        {
            // minus drebezg
            button_count++;
        }
        switch (button_count)
        {
        case 1:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 2:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 3:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 4:
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); // Установка единицы в 7-ой бит регистра ODR
            break;
        case 5:
            // сбросить все лампы (_БР)
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); // Установка единицы в 7-ой бит регистра ODR
            break;
        default:
            // зажечь все лампы (_БС)
            button_count= 0;
            break;
        }*/
    }
}