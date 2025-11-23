#include "../Inc/interrupt.h"

extern uint16_t TickCounter;
extern uint16_t ButtonTicks;
extern bool LedState;
uint16_t DelayCounter = 0;
uint16_t Inerticker = 0;
// режим свечения
uint8_t LED_MODE;
// номер светящегося
extern uint8_t LED_NUM;
// который настраиваем
// shine
// mode
extern uint8_t LED_OPTION[2][7];
extern uint8_t LED_OPT;
// частота в режиме
uint8_t shine_mode;
uint8_t mode_mode;
bool but1_press = false;
bool but2_press = false;

extern uint16_t freq[3][3];

void SysTick_Handler(void)
{
    TickCounter++;
    ButtonTicks++;
}

void Delay(uint16_t delay)
{
    while (TickCounter < delay)
        ;
    if (TickCounter >= delay)
        TickCounter = 0;
}

void EXTI3_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR3);
    if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID3) == RESET)
    {
        but1_press = !but1_press;
        ButtonTicks = 0;
    }
    if (ButtonTicks >= 2000)
    {
        but1_press = !but1_press;
        LED_OPTION[0][LED_OPT]++;
        if (LED_OPTION[0][LED_OPT] > 2)
            LED_OPTION[0][LED_OPT] = 0;
        shine_mode = LED_OPTION[0][LED_OPT];
        ButtonTicks = 0;
    }
    else if (ButtonTicks < 2000 && ButtonTicks > 70)
    {
        but1_press = !but1_press;
        LED_NUM++;
        if (LED_NUM > 6)
            LED_NUM = 0;
        ButtonTicks = 0;
    }
}

void EXTI2_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR2);
    if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID2) == RESET)
    {
        but2_press = !but2_press;
        ButtonTicks = 0;
    }
    if (ButtonTicks >= 2000)
    {
        but2_press = !but2_press;
        LED_OPTION[1][LED_OPT]++;
        if (LED_OPTION[1][LED_OPT] > 2)
            LED_OPTION[1][LED_OPT] = 0;
        mode_mode = LED_OPTION[1][LED_OPT];
        ButtonTicks = 0;
    }
    else if (ButtonTicks < 2000 && ButtonTicks > 70)
    {
        but2_press = !but2_press;
        LED_OPT++;
        if (LED_OPT > 5)
            LED_OPT = 0;
        ButtonTicks = 0;
    }
}