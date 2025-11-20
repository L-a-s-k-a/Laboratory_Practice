#include "../Inc/init.h"
#include <stdint.h>

int main(void)
{
    Init_PORT_D_withMemory();                                                               // PD1 - LED (синий)
    Init_PORT_A_withMemory();                                                               // PA0 - LED (зелёный)
    Init_PORT_E_withMemory();                                                               // PE0 - LED (красный)

    Init_Port_D_withMemory_Button();                                                        // PD0 - кнопка 1
    Init_Port_A_withMemory_Button();                                                        // PA4 - кнопка 2
    Init_Port_E_withMemory_Button();                                                        // PE2 - кнопка 3

    Init_Port_C_with_Memory_Button();                                                       // PC13 - кнопка 4

    volatile uint8_t rot    = 0;            
    volatile uint8_t prevB4 = 0; 

    while (1)
    {
         volatile uint8_t b1 = ((*(uint32_t*)(0x40020C00UL + 0x10UL)) >> 0 ) & 1U;         // PD0
        uint8_t b2 = ((*(uint32_t*)(0x40020000UL + 0x10UL)) >> 4 ) & 1U;                   // PA4
        uint8_t b3 = ((*(uint32_t*)(0x40021000UL + 0x10UL)) >> 2 ) & 1U;                   // PE2
        uint8_t c1 = ((*(uint32_t*)(0x40020800UL + 0x10UL)) >> 13) & 1U;                   // PC13 (первое чтение)

        for (volatile int d=0; d<2000; ++d) __asm__("nop");
        uint8_t c2 = ((*(uint32_t*)(0x40020800UL + 0x10UL)) >> 13) & 1U;

        uint8_t b4 = (c1 == c2) ? c2 : 0;                                              

        if (prevB4 == 0 && b4 == 1)
        {
            rot++;
            if (rot >= 3) rot = 0;
        }
        prevB4 = b4;

        // Сначала выключаем все LED 
        *(uint32_t*)(0x40020000UL + 0x18UL) = (1UL << (0+16));                              // PA0 
        *(uint32_t*)(0x40020C00UL + 0x18UL) = (1UL << (1+16));                              // PD1 
        *(uint32_t*)(0x40021000UL + 0x18UL) = (1UL << (0+16));                              // PE0 

        // Обработка кнопок 1–3 через функцию
        HandleButton(b1, 0, rot);  // кнопка 1 — base=0
        HandleButton(b2, 1, rot);  // кнопка 2 — base=1
        HandleButton(b3, 2, rot);  // кнопка 3 — base=2
    }
}

//-----------------------КОД ДЛЯ ЗАЩИТЫ-----------------------//

/*
#include "../Inc/init.h"

//частота
#define HPERIOD_F1  200000UL 
#define HPERIOD_F2   80000UL   
#define HPERIOD_F3   300000UL  

int main(void)
{
    Init_PORT_D_withMemory();      // PD1 - зелёный
    Init_PORT_A_withMemory();      // PA0 - синий
    Init_PORT_E_withMemory();      // PE0 - красный

    Init_Port_D_withMemory_Button(); // PD0 - кнопка 1
    Init_Port_A_withMemory_Button(); // PA4 - кнопка 2
    Init_Port_E_withMemory_Button(); // PE2 - кнопка 3
    Init_Port_C_with_Memory_Button(); // PC13 - кнопка 4

    uint8_t rot = 0;    //счетчик нажатия кнопки 0..2

    uint8_t prevB1 = 1, prevB2 = 1, prevB3 = 1, prevB4 = 1;

    uint8_t activeLed = 0xFF;
    // Индекс частоты: 0..2
    uint8_t freqIndex = 0;

    while (1)
    {
        uint8_t b1 = ((*(uint32_t*)(0x40020C00UL + 0x10UL)) >> 0 ) & 1U;  // PD0
        uint8_t b2 = ((*(uint32_t*)(0x40020000UL + 0x10UL)) >> 4 ) & 1U;  // PA4
        uint8_t b3 = ((*(uint32_t*)(0x40021000UL + 0x10UL)) >> 2 ) & 1U;  // PE2

        uint8_t c1 = ((*(uint32_t*)(0x40020800UL + 0x10UL)) >> 13) & 1U;  // PC13
        
        for (volatile int d=0; d<2000; ++d) __asm__("nop");
        uint8_t c2 = ((*(uint32_t*)(0x40020800UL + 0x10UL)) >> 13) & 1U;
        uint8_t b4 = (c1==c2) ? c2 : 0;

        if (prevB4 == 0 && b4 == 1)
         {
            rot++; if (rot >= 3) rot = 0;
            if (activeLed != 0xFF) 
            {
              if (activeLed == 0) *(uint32_t*)(0x40020C00UL + 0x18UL) = (1UL << (1+16)); // PD1 OFF
              if (activeLed == 1) *(uint32_t*)(0x40020000UL + 0x18UL) = (1UL << (0+16)); // PA0 OFF
              if (activeLed == 2) *(uint32_t*)(0x40021000UL + 0x18UL) = (1UL << (0+16)); // PE0 OFF
              activeLed = 0xFF;
            }
        }
        
        handle_button(0, b1, b4, rot, &prevB1, &prevB4, &activeLed, &freqIndex);
        handle_button(1, b2, b4, rot, &prevB2, &prevB4, &activeLed, &freqIndex);
        handle_button(2, b3, b4, rot, &prevB3, &prevB4, &activeLed, &freqIndex);

        blink_active_led(activeLed, freqIndex);
    }
}
*/