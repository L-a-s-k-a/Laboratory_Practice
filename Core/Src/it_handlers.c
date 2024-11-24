#include "it_handlers.h"
extern uint8_t LedState, BtnCount1, LongBtnCount1, BtnCount2, LongBtnCount2; // extern - обозначает, что переменная будет объявленна в другом месте (в файле main.c)
extern uint16_t GlobalTickCount;
uint16_t ExternInterruptTickCount, ExternInterruptTickCount2, DelayTickCount, dTime1, dTime2;
uint8_t front1, front2;

void SysTick_Handler(void)
{ // прерывание по таймеру
    ExternInterruptTickCount++;
    ExternInterruptTickCount2++;
    GlobalTickCount++;
}
void EXTI15_10_IRQHandler(void)
{                                    // прерывание по кнопке
    SET_BIT(EXTI->PR, EXTI_PR_PR13); // Сброс флага прерывания
    if (ExternInterruptTickCount >= DELAY_BUTTON_FILTER)
    { // Выполнится, когда пройдёт 100 мс с момента обнуления данной переменной
        front1 = !front1;
        if (front1 == 1)
            dTime1 = GlobalTickCount;
        else
        {
            if (GlobalTickCount - (dTime1 - 1) >= 2001)
                LongBtnCount1++; // если разница между фронтами нажатия больше 2секунд, то долгого нажатия
            else
                BtnCount1++; // Изменение состояния кнопки
        }
        ExternInterruptTickCount = 0; // Обнуление счётчика таймера
    }
}
void EXTI9_5_IRQHandler(void)
{                                   // прерывание по кнопке
    SET_BIT(EXTI->PR, EXTI_PR_PR9); // Сброс флага прерывания
    if (ExternInterruptTickCount2 >= DELAY_BUTTON_FILTER + 100)
    { // Выполнится, когда пройдёт 100 мс с момента обнуления данной переменной
        front2 = !front2;
        if (front2 == 1)
            dTime2 = GlobalTickCount;
        else
        {
            if (GlobalTickCount - (dTime2 - 1) >= 2001)
                LongBtnCount2++; // если разница между фронтами нажатия больше 2секунд, то долгого нажатия
            else
                BtnCount2++; // Изменение состояния кнопки
        }
        ExternInterruptTickCount2 = 0; // Обнуление счётчика таймера
    }
}
void User_Delay(uint32_t delay)
{ // пользовательская функция
    while (DelayTickCount < delay)
    {
    } // Цикл, благодаря которому происходит задержка программы
    if (DelayTickCount >= delay)
        DelayTickCount = 0; // Обнуление переменной счётчика, при достижении заданного пользователем значения
}