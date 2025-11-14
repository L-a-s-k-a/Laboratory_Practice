#include "init.h"
#include "it_handlers.h"

/* int main(void)
{
    while(1)
    {
        
    }
} */
    
int main(void) // ОСНОВНОЕ ЗАДАНИЕ
{
    int counter = -1;
    uint8_t check_status10 = 1; // 0 - кнопка не нажата, 1 - кнопка нажата
    uint8_t check_status1 = 0;
    uint8_t check_status2 = 0;
    uint8_t check_status3 = 0;
    uint8_t flag = 0;
    GPIO_Init_with_CMSIS();
    GPIO_Init_with_my_macro();
    Init_with_memory_for_led3();
    while (1)
    {
        if (READ_GPIO_C10 != 0) // если главная кнопка нажата
        {
            delay(300000);
            if((READ_GPIO_C10 != 0) && (check_status10 == 0)) // если главная кнопка нажата и до этого она была не нажата
            {
                counter = (counter + 1) % 5; // 5 значений -- 0 1 2 3 4
                check_status10 = 1;
            }
        }
        else
        {
            check_status10 = 0; // сброс счётчика
        }

        /////////////////////////////////////////////////////////////////////

        if (flag == 0)
        {
            switch (counter)
        {
        case 0: // первое нажатие
            GPIOB_MODE_PIN1_OUT; // переводим PB1 в состояние выхода
            break;
        case 1: // второе нажатие
            GPIOB_MODE_PIN2_OUT; // переводим PB2 в состояние выхода
            break;
        case 2: // третье нажатие
            GPIOB_MODE_PIN3_OUT; // переводим PB3 в состояние выхода
            break;
        case 3: // четвёртое нажатие
            GPIOB_MODE_PIN1_IN; // переводим PB1 в состояние входа
            GPIOB_MODE_PIN2_IN; // переводим PB2 в состояние входа
            GPIOB_MODE_PIN3_IN; // переводим PB3 в состояние входа
            break;
        case 4: // пятое нажатие
            GPIOB_MODE_PIN1_OUT; // переводим PB1 в состояние выхода
            GPIOB_MODE_PIN2_OUT; // переводим PB2 в состояние выхода
            GPIOB_MODE_PIN3_OUT; // переводим PB3 в состояние выхода
            flag++;
            break;
        default:
            break;
        }
        }
        else
        {
            switch (counter)
        {
        case 0: // первое нажатие
            GPIOB_MODE_PIN1_IN; // переводим PB1 в состояние входа
            break;
        case 1: // второе нажатие
            GPIOB_MODE_PIN2_IN; // переводим PB2 в состояние входа
            break;
        case 2: // третье нажатие
            GPIOB_MODE_PIN3_IN; // переводим PB3 в состояние входа
            break;
        case 3: // четвёртое нажатие
            GPIOB_MODE_PIN1_IN; // переводим PB1 в состояние входа
            GPIOB_MODE_PIN2_IN; // переводим PB2 в состояние входа
            GPIOB_MODE_PIN3_IN; // переводим PB3 в состояние входа
            break;
        case 4: // пятое нажатие
            GPIOB_MODE_PIN1_OUT; // переводим PB1 в состояние выхода
            GPIOB_MODE_PIN2_OUT; // переводим PB2 в состояние выхода
            GPIOB_MODE_PIN3_OUT; // переводим PB3 в состояние выхода
            break;
        default:
            break;
        }
        }

        /////////////////////////////////////////////////////////////////////

        // ПЕРВАЯ КНОПКА РЕЖИМ ВХОДА

        if (READ_CONDITION_PORT_B1 == 0) // если пин работает в режиме входа
        {
            if (READ_GPIO_B1 != 0) // если кнопка нажата
            {
                SET_GPIO_A5; // типо жёлтый
                delay(300000);
            if((READ_GPIO_B1 != 0) && (check_status1 == 0)) // если кнопка нажата и до этого она была не нажата
            { 
                check_status1 = 1;
            }
            else
            {
                RESET_GPIO_A5;
            }
            }
        }
        else
        {
            check_status1 = 0; // сброс счётчика
            RESET_GPIO_A5;
        }

        ///////

        // ПЕРВАЯ КНОПКА РЕЖИМ ВЫХОДА

        if (READ_CONDITION_PORT_B1 == 1)  // если пин работает в режиме выхода
        {
            SET_GPIO_B1;
        }
        else
        {
            RESET_GPIO_B1;
        }

        /////////////////////////////////////////////////////////////////////

        // ВТОРАЯ КНОПКА РЕЖИМ ВХОДА

        if (READ_CONDITION_PORT_B2 == 0) // если пин работает в режиме входа
        {
            if (READ_GPIO_B2 != 0) // если кнопка нажата
            {
                SET_GPIO_A10; // типо синий
                delay(300000);
            if((READ_GPIO_B2 != 0) && (check_status2 == 0)) // если кнопка нажата и до этого она была не нажата
            { 
                check_status2 = 1;
            }
            else
            {
                RESET_GPIO_A10;
            }
            }
        }
        else
        {
            check_status2 = 0; // сброс счётчика
            RESET_GPIO_A10;
        }

        ///////

        // ВТОРАЯ КНОПКА РЕЖИМ ВЫХОДА

        if (READ_CONDITION_PORT_B2 == 1)  // если пин работает в режиме выхода
        {
            SET_GPIO_B2;
        }
        else
        {
            RESET_GPIO_B2;
        }
        
        /////////////////////////////////////////////////////////////////////

        // ТРЕТЬЯ КНОПКА РЕЖИМ ВХОДА

        if (READ_CONDITION_PORT_B3 == 0) // если пин работает в режиме входа
        {
            if (READ_GPIO_B3 != 0) // если кнопка нажата
            {
                SET_GPIO_A11; // типо синий
                delay(300000);
            if((READ_GPIO_B3 != 0) && (check_status3 == 0)) // если кнопка нажата и до этого она была не нажата
            { 
                check_status3 = 1;
            }
            else
            {
                RESET_GPIO_A11;
            }
            }
        }
        else
        {
            check_status2 = 0; // сброс счётчика
            RESET_GPIO_A11;
        }

        ///////

        // ТРЕТЬЯ КНОПКА РЕЖИМ ВЫХОДА

        if (READ_CONDITION_PORT_B3 == 1)  // если пин работает в режиме выхода
        {
            SET_GPIO_B3;
        }
        else
        {
            RESET_GPIO_B3;
        }
    }
}

/* int main(void) // ДОПОЛНИТЕЛЬНОЕ ЗАДАНИЕ
{
    int counter10 = 0; // счётчик для главной кнопки
    int counter1 = -1; // счётчик для первой кнопки
    int counter2 = -1; // счётчик для второй кнопки
    int counter3 = -1; // счётчик для третьей кнопки
    uint8_t check_status10 = 0; // 0 - кнопка не нажата, 1 - кнопка нажата
    uint8_t check_status1 = 0;
    uint8_t check_status2 = 0;
    uint8_t check_status3 = 0;
    GPIO_Init_with_my_macro();
    while (1)
    {
        if ((*(uint32_t*)(0x40020800 + 0x10UL) & 0x400) != 0) // если главная кнопка нажата
        {
            delay(300000);
            if((READ_GPIO_C10 != 0) && (check_status10 == 0)) // если кнопка нажата и до этого она была не нажата
            {
                counter10 = (counter10 + 1) % 3; // 3 значения -- 0 1 2
                check_status10 = 1;
            }
        }
        else
        {
            check_status10 = 0; // сброс счётчика
        }

        /////////////////////////////////////////////////////////////////////

        if (READ_GPIO_B1 != 0) // если первая кнопка нажата
        {
            delay(300000);
            if((READ_GPIO_B1 != 0) && (check_status1 == 0)) // если кнопка нажата и до этого она была не нажата
            {
                counter1 = (counter1 + 1) % 5; // 5 значений -- 0 1 2 3 4
                check_status1 = 1;
            }
        }
        else
        {
            check_status1 = 0; // сброс счётчика
        }

        /////////////////////////////////////////////////////////////////////

        if (READ_GPIO_B2 != 0) // если вторая кнопка нажата
        {
            delay(300000);
            if((READ_GPIO_B2 != 0) && (check_status2 == 0)) // если кнопка нажата и до этого она была не нажата
            {
                counter2 = (counter2 + 1) % 5; // 5 значений -- 0 1 2 3 4
                check_status2 = 1;
            }
        }
        else
        {
            check_status2 = 0; // сброс счётчика
        }

        /////////////////////////////////////////////////////////////////////

        if (READ_GPIO_B3 != 0) // если третья кнопка нажата
        {
            delay(300000);
            if((READ_GPIO_B3 != 0) && (check_status3 == 0)) // если кнопка нажата и до этого она была не нажата
            {
                counter3 = (counter3 + 1) % 5; // 5 значений -- 0 1 2 3 4
                check_status3 = 1;
            }
        }
        else
        {
            check_status3 = 0; // сброс счётчика
        }

        /////////////////////////////////////////////////////////////////////

        if(counter10 == 0)
        {
            switch (counter1)
        {
        case 0:
            SET_GPIO_A5;
            break;
        case 1:
            SET_GPIO_A5;
            delay(500000);
            RESET_GPIO_A5;
            delay(500000);
            break;
        case 2:
            SET_GPIO_A5;
            delay(250000);
            RESET_GPIO_A5;
            delay(250000);
            SET_GPIO_A5;
            delay(250000);
            RESET_GPIO_A5;
            delay(250000);
            break;
        case 3:
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            break;
        case 4:
            RESET_GPIO_A5;
            break;
        default:
            break;
        }

        switch (counter2)
        {
        case 0:
            SET_GPIO_A10;
            break;
        case 1:
            SET_GPIO_A10;
            delay(500000);
            RESET_GPIO_A10;
            delay(500000);
            break;
        case 2:
            SET_GPIO_A10;
            delay(250000);
            RESET_GPIO_A10;
            delay(250000);
            SET_GPIO_A10;
            delay(250000);
            RESET_GPIO_A10;
            delay(250000);
            break;
        case 3:
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            break;
        case 4:
            RESET_GPIO_A10;
            break;
        default:
            break;
        }

        switch (counter3)
        {
        case 0:
            SET_GPIO_A11;
            break;
        case 1:
            SET_GPIO_A11;
            delay(500000);
            RESET_GPIO_A11;
            delay(500000);
            break;
        case 2:
            SET_GPIO_A11;
            delay(250000);
            RESET_GPIO_A11;
            delay(250000);
            SET_GPIO_A11;
            delay(250000);
            RESET_GPIO_A11;
            delay(250000);
            break;
        case 3:
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            break;
        case 4:
            RESET_GPIO_A11;
            break;
        default:
            break;
        }
        }

        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////

        if(counter10 == 1)
        {
            switch (counter2)
        {
        case 0:
            SET_GPIO_A5;
            break;
        case 1:
            SET_GPIO_A5;
            delay(500000);
            RESET_GPIO_A5;
            delay(500000);
            break;
        case 2:
            SET_GPIO_A5;
            delay(250000);
            RESET_GPIO_A5;
            delay(250000);
            SET_GPIO_A5;
            delay(250000);
            RESET_GPIO_A5;
            delay(250000);
            break;
        case 3:
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            break;
        case 4:
            RESET_GPIO_A5;
            break;
        default:
            break;
        }

        switch (counter3)
        {
        case 0:
            SET_GPIO_A10;
            break;
        case 1:
            SET_GPIO_A10;
            delay(500000);
            RESET_GPIO_A10;
            delay(500000);
            break;
        case 2:
            SET_GPIO_A10;
            delay(250000);
            RESET_GPIO_A10;
            delay(250000);
            SET_GPIO_A10;
            delay(250000);
            RESET_GPIO_A10;
            delay(250000);
            break;
        case 3:
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            break;
        case 4:
            RESET_GPIO_A10;
            break;
        default:
            break;
        }

        switch (counter1)
        {
        case 0:
            SET_GPIO_A11;
            break;
        case 1:
            SET_GPIO_A11;
            delay(500000);
            RESET_GPIO_A11;
            delay(500000);
            break;
        case 2:
            SET_GPIO_A11;
            delay(250000);
            RESET_GPIO_A11;
            delay(250000);
            SET_GPIO_A11;
            delay(250000);
            RESET_GPIO_A11;
            delay(250000);
            break;
        case 3:
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            break;
        case 4:
            RESET_GPIO_A11;
            break;
        default:
            break;
        }
        }

        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////

        if(counter10 == 2)
        {
            switch (counter3)
        {
        case 0:
            SET_GPIO_A5;
            break;
        case 1:
            SET_GPIO_A5;
            delay(500000);
            RESET_GPIO_A5;
            delay(500000);
            break;
        case 2:
            SET_GPIO_A5;
            delay(250000);
            RESET_GPIO_A5;
            delay(250000);
            SET_GPIO_A5;
            delay(250000);
            RESET_GPIO_A5;
            delay(250000);
            break;
        case 3:
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            SET_GPIO_A5;
            delay(125000);
            RESET_GPIO_A5;
            delay(125000);
            break;
        case 4:
            RESET_GPIO_A5;
            break;
        default:
            break;
        }

        switch (counter1)
        {
        case 0:
            SET_GPIO_A10;
            break;
        case 1:
            SET_GPIO_A10;
            delay(500000);
            RESET_GPIO_A10;
            delay(500000);
            break;
        case 2:
            SET_GPIO_A10;
            delay(250000);
            RESET_GPIO_A10;
            delay(250000);
            SET_GPIO_A10;
            delay(250000);
            RESET_GPIO_A10;
            delay(250000);
            break;
        case 3:
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            SET_GPIO_A10;
            delay(125000);
            RESET_GPIO_A10;
            delay(125000);
            break;
        case 4:
            RESET_GPIO_A10;
            break;
        default:
            break;
        }

        switch (counter2)
        {
        case 0:
            SET_GPIO_A11;
            break;
        case 1:
            SET_GPIO_A11;
            delay(500000);
            RESET_GPIO_A11;
            delay(500000);
            break;
        case 2:
            SET_GPIO_A11;
            delay(250000);
            RESET_GPIO_A11;
            delay(250000);
            SET_GPIO_A11;
            delay(250000);
            RESET_GPIO_A11;
            delay(250000);
            break;
        case 3:
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            SET_GPIO_A11;
            delay(125000);
            RESET_GPIO_A11;
            delay(125000);
            break;
        case 4:
            RESET_GPIO_A11;
            break;
        default:
            break;
        }
        }
    }
} */

/*
    2 ЛАБОРАТОРНАЯ РАБОТА
1. Нужно очистить все регистры которые используем
2. Выключение HSI (внутреннего тактирования)
3. Включить источник тактирования HSE и узнать какой там осциллятор
4. 
csson - система безопасности таймера
pll - блок который умножает входную частоту
*/