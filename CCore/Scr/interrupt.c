#include "../Inc/interrupt.h"

extern uint16_t FrequencyTickCount;
extern bool LedMode;
extern uint16_t ButtonTickCount;
extern uint8_t Frequency;
bool clicking_button = false;

void SysTick_Handler(void)
{
    FrequencyTickCount++;
    ButtonTickCount++;
}

void EXTI3_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR3);                             // Выход с обработчика прерываний
    if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID3) == RESET)            // Если кнопка нажата, то обнуляется таймер
    {
        clicking_button = !clicking_button;
        ButtonTickCount = 0;
    }
    else if (ButtonTickCount >= 2000)                           // Если после отпускание кнопки прошло больше 2 сек., то частота мерцания меняется
    {
        clicking_button = !clicking_button;
        Frequency++;
        if (Frequency >= 3)
        {
            Frequency = 0;
        }
        FrequencyTickCount = 0;
        ButtonTickCount = 0;
    }
    else if (ButtonTickCount >= 50 && ButtonTickCount <= 2000)  // Если после отпускание кнопки прошло менее 2 сек., то меняется режим работы
    {
        clicking_button = !clicking_button;
        LedMode = !LedMode;
        Frequency = 0;
        FrequencyTickCount = 0;
        ButtonTickCount = 0;
    }
}