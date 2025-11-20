 #include "../Inc/init.h"

 // Есть порты: portA, portB, portC, portD
 //частота
#define HPERIOD_F1  200000UL 
#define HPERIOD_F2   80000UL   
#define HPERIOD_F3   300000UL 

//(светодиод pd1)
void Init_PORT_D_withMemory(void) 
{
    (*(uint32_t*)(0x40023800+0x30UL)) |= 0x08UL; 
    (*(uint32_t*)(0x40020C00UL + 0x00UL)) |= (0b0100UL); 
    (*(uint32_t*)(0x40020C00UL + 0x04UL)) |= 0b00UL;  
    (*(uint32_t*)(0x40020C00UL + 0x08UL)) |= (0b0100UL); 
    (*(uint32_t*)(0x40020C00UL + 0x0CUL) )|= (0b00UL); 
    (*(uint32_t*)(0x40020C00UL + 0x18UL)) |= (0x20000UL); 
}

//светодиод PA0 через CMSIS
void Init_PORT_A_withMemory(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);        //тактирование
    SET_BIT(GPIOA->MODER, GPIO_MODER_MODE0_0);         //настроили на выход MODER
    CLEAR_BIT(GPIOA->OTYPER, GPIO_OTYPER_OT0);         //OTYPER настраивается тип выход, по умолчанию push-pull как нам и надо
    SET_BIT(GPIOA->OSPEEDR, GPIO_OSPEEDER_OSPEEDR0_0); //Скорость medium
    CLEAR_BIT(GPIOA->PUPDR, GPIO_PUPDR_PUPD0);         //no pull-resistor
    SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR0);               //BSRR выключили LED PA0
}

//светодиод PE0 через собственные макросы

void Init_PORT_E_withMemory(void)
{
    MY_SET_BIT(RCC_GPIO_EN, RCC_GPIOE_EN);
    MY_SET_BIT(GPIOE_MODER_REG, GPIOE_MODER_BIT);
    MY_SET_BIT(GPIOE_OTYPER_REG, GPIOE_OTYPER_BIT);
    MY_SET_BIT(GPIOE_OSPEEDR_REG, GPIOE_OSPEEDR_BIT);
    MY_SET_BIT(GPIOE_PUPDR_REG, GPIOE_PUPDR_BIT);   
    MY_SET_BIT(GPIOE_BSRR_REG, GPIOE_BSRR_BIT);
}

//кнопка PD0
void Init_Port_D_withMemory_Button(void)
{ 
    (*(uint32_t*)(0x40020C00UL + 0x00UL)) |= 0b00UL;                 //настроили на вход MODER
    (*(uint32_t*)(0x40020C00UL + 0x04UL)) |= 0b0UL;                   //OTYPER настраивается тип выход, по умолчанию push-pull как нам и надо
    (*(uint32_t*)(0x40020C00UL + 0x0CUL)) |= 0b01UL;                    //  pull-up resistor
}

//кнопка PA4
void Init_Port_A_withMemory_Button(void)
{ 
   *(uint32_t*)(0x40020000UL + 0x00UL) &= ~0xC000UL;                    //Явное обнуление, BIT_CLEAR
   *(uint32_t*)(0x40020000UL + 0x04UL) &= ~0x80UL;                      //OTYPER настраивается тип выход, по умолчанию push-pull как нам и надо
   *(uint32_t*)(0x40020000UL + 0x0CUL) &= ~0xC000UL;                    // Сначала очищаем
    (*(uint32_t*)(0x40020000UL + 0x0CUL)) |= 0b0000100000000UL;          // Затем устанавливаем pull-up (01)

}

//кнопка PE2
void Init_Port_E_withMemory_Button(void)
{
    *(uint32_t*)(0x40021000UL + 0x00UL) &= ~0x30UL; 
    *(uint32_t*)(0x40021000UL + 0x04UL) &= ~0x4UL;                          //OTYPER настраивается тип выход, по умолчанию push-pull как нам и надо
    *(uint32_t*)(0x40021000UL + 0x0CUL) |= 0x10UL;                      //  pull-resistor
}

//кнопка PC13
void Init_Port_C_with_Memory_Button(void)
{
    (*(uint32_t*)(0x40023800 + 0x30UL)) |= 0x04UL;                      // тактирование порта C
    *(uint32_t*)(0x40020800UL + 0x00UL) &= ~0xC000000UL;                 // MODER13 = 00
    *(uint32_t*)(0x40020800UL + 0x04UL) &= ~0x2000UL;
    *(uint32_t*)(0x40020800UL + 0x0CUL) |=  0x8000000;               // PUPDR13 = 01 (pull-up)
}

// Функция обработки кнопки 1–3

void HandleButton(uint8_t btnState, uint8_t base, uint8_t rot)
{

    if (btnState != 0) return;

    uint8_t tgt = base + rot;
    tgt = (base + rot) % 3;

    if (tgt == 0)
    {
        // PD1 ON
        *(uint32_t*)(0x40020C00UL + 0x18UL) = (1UL << 1);
    }
    else if (tgt == 1)
    {
        // PA0 ON
        *(uint32_t*)(0x40020000UL + 0x18UL) = (1UL << 0);
    }
    else // tgt == 2
    {
        // PE0 ON
        *(uint32_t*)(0x40021000UL + 0x18UL) = (1UL << 0);
    }
}


//-----------------------КОД ДЛЯ ЗАЩИТЫ-----------------------//

void blink_active_led(uint8_t activeLed, uint8_t freqIndex)
{
    if (activeLed != 0xFF) 
    {
        uint32_t h = (freqIndex == 0) ? HPERIOD_F1 : (freqIndex == 1) ? HPERIOD_F2 : HPERIOD_F3;

        for (volatile uint32_t i = 0; i < h; ++i) __asm__("nop");

        // инвертируем выбранный светодиод через ODR/BSRR
        if (activeLed == 0) 
        {
            uint32_t odr = (*(uint32_t*)(0x40020C00UL + 0x14UL) >> 1) & 1U; // PD1
            if (odr) *(uint32_t*)(0x40020C00UL + 0x18UL) = (1UL << (1+16)); 
            else     *(uint32_t*)(0x40020C00UL + 0x18UL) = (1UL << 1);
        }
        else if (activeLed == 1) 
        {
            uint32_t odr = (*(uint32_t*)(0x40020000UL + 0x14UL) >> 0) & 1U; // PA0
            if (odr) *(uint32_t*)(0x40020000UL + 0x18UL) = (1UL << (0+16));
            else     *(uint32_t*)(0x40020000UL + 0x18UL) = (1UL << 0);
        } 
        else if (activeLed == 2)
        {
            uint32_t odr = (*(uint32_t*)(0x40021000UL + 0x14UL) >> 0) & 1U; // PE0
            if (odr) *(uint32_t*)(0x40021000UL + 0x18UL) = (1UL << (0+16)); 
            else     *(uint32_t*)(0x40021000UL + 0x18UL) = (1UL << 0);
        }
    } 
    else 
    {
        // никто не активен - погасим всё и маленькая пауза
        *(uint32_t*)(0x40020000UL + 0x18UL) = (1UL << (0+16));
        *(uint32_t*)(0x40020C00UL + 0x18UL) = (1UL << (1+16));
        *(uint32_t*)(0x40021000UL + 0x18UL) = (1UL << (0+16));
        for (volatile int d=0; d<2000; ++d) __asm__("nop");
    }
}

void handle_button(uint8_t btnNum,   // номер кнопки: 0, 1, 2 (для кнопок 1,2,3)
                          uint8_t b,        // текущее состояние кнопки (0/1)
                          uint8_t b4,       // состояние кнопки 4 (для prevB4)
                          uint8_t rot, 
                          uint8_t *prevB, 
                          uint8_t *prevB4,
                          uint8_t *activeLed,
                          uint8_t *freqIndex)
{
    // обновляем состояние кнопки 4
    *prevB4 = b4;

    if (*prevB == 1 && b == 0)  
    {
        uint8_t tgt = btnNum + rot; 
        if (tgt >= 3) tgt -= 3; 

        if (*activeLed != tgt) 
        {
            // гасим предыдущий светодиод
            if (*activeLed == 0) *(uint32_t*)(0x40020C00UL + 0x18UL) = (1UL << (1+16));
            if (*activeLed == 1) *(uint32_t*)(0x40020000UL + 0x18UL) = (1UL << (0+16));
            if (*activeLed == 2) *(uint32_t*)(0x40021000UL + 0x18UL) = (1UL << (0+16));

            *activeLed = tgt;
            *freqIndex = 0;  // начинаем с первой частоты

            if (*activeLed == 0) *(uint32_t*)(0x40020C00UL + 0x18UL) = (1UL << 1);
            if (*activeLed == 1) *(uint32_t*)(0x40020000UL + 0x18UL) = (1UL << 0);
            if (*activeLed == 2) *(uint32_t*)(0x40021000UL + 0x18UL) = (1UL << 0);
        } 
        else 
        {
            // повторное нажатие — смена частоты
            (*freqIndex)++;
            if (*freqIndex >= 3) *freqIndex = 0;
        }
    }

    *prevB = b;
}