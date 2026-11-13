#include "init.h"

uint8_t led_count = 0; // Количество включенных светодиодов (0..6)
uint8_t swap_mode = 0; // Режим кнопок (0 - обычный, 1 - кнопки 1 и 2 поменяны местами)

int main(void)
{
    GPIO_Ini();

    // Гасим все 6 светодиодов на старте через старшие биты регистра BSRR
    *(uint32_t*)(0x40020400UL + 0x18UL) |= 0xC0870000UL;

    uint8_t btn1_flag = 0;
    uint8_t btn2_flag = 0;
    uint8_t btn3_flag = 0;

    // Запоминаем исходное состояние внешних кнопок PC0 и PC3
    uint32_t btn2_start = (*(uint32_t*)(0x40020800UL + 0x10UL) & 0x0001UL);
    uint32_t btn3_start = (*(uint32_t*)(0x40020800UL + 0x10UL) & 0x0008UL);

    while (1)
    {
        // кнопка 3
        if ((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x0008UL) != btn3_start)
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
        if ((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x2000UL) != 0)
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
        if ((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x0001UL) != btn2_start)
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

        //  Включение/выключение светодиодов через регистр BSRR 
        if (led_count >= 1) *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x0001UL; else *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x00010000UL; // PB0
        if (led_count >= 2) *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x0080UL; else *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x00800000UL; // PB7
        if (led_count >= 3) *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x4000UL; else *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x40000000UL; // PB14
        if (led_count >= 4) *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x0002UL; else *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x00020000UL; // PB1
        if (led_count >= 5) *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x0004UL; else *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x00040000UL; // PB2
        if (led_count >= 6) *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x8000UL; else *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80000000UL; // PB15

        // Задержка от дребезга контактов
        for (volatile int i = 0; i < 30000; i++);
    }
}