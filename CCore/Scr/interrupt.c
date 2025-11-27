#include "../Inc/interrupt.h"

uint8_t counter1 = 0;
extern uint8_t type;
extern uint32_t counter_timer;
uint32_t Frequency_1Hz = 1000;
uint32_t Frequency_2Hz = 500;
uint32_t Frequency_4Hz = 250;

void EXTI15_10_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR13);                  // Выход с обработчика прерываний
    if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) == RESET) // Если кнопка нажата, то обнуляется таймер
    {
    }
    else
    {
        counter_timer = 0;
        counter1++;
        if (counter1 > 2)
        {
            counter1 = 0;
        }
    }
}

void EXTI3_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR3); // Выход с обработчика прерываний
    if (READ_BIT(GPIOC->IDR, GPIO_IDR_ID3) == RESET)
    {
    }
    else
    {
        type++;
        if (type > 4)
        {
            type = 0;
        }
    }
}

void TIM1_UP_TIM10_IRQHandler(void)
{
    if (TIM10->SR & TIM_SR_UIF)
    {                             // Проверка флага обновления
        TIM10->SR &= ~TIM_SR_UIF; // Сброс флага
        counter_timer++;          // Увеличиваем счетчик каждую 1 мс
        switch (counter1)
        {
        case 0:
            Flickering_LED(Frequency_1Hz);
            break;
        case 1:
            Flickering_LED(Frequency_2Hz);
            break;
        case 2:
            Flickering_LED(Frequency_4Hz);
            break;
        }
    }
}