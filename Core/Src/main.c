#include "init.h"
#include "Interrupt.h"

int main(void)
{
    /* Тактирование на 168 МГц */
    Clock_Init_HSE_PLL_168MHz();

    /* SysTick 1 мс для антидребезга */
    SysTick_Init_1ms();

    /* GPIO + EXTI0 */
    GPIO_EXTI_Init();

    /* TIM2 (ещё не запущен) */
    TIM2_Init();

    /* По умолчанию всё выключено, гирлянда не крутится */
    LEDs_AllOff();
    g_running = 0U;
    g_currentLed = 0U;
    g_speedIndex = 0U;

    while (1)
    {
        g_currentLed;
        /* Спим до прерывания (TIM2 / EXTI / SysTick) */
        __WFI();
    }
}
