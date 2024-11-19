#include "it_handlers.h"
extern uint8_t LedState, BtnCount1, LongBtnCount1; // extern - обозначает, что переменная будет объявленна в другом месте (в файле main.c)
extern uint16_t GlobalTickCount;
uint16_t ExternInterruptTickCount, DelayTickCount, dTime;
void SysTick_Handler(void)
{ // прерывание по таймеру
    ExternInterruptTickCount++;
    GlobalTickCount++;
}
void EXTI15_10_IRQHandler(void)
{                                    // прерывание по кнопке
    SET_BIT(EXTI->PR, EXTI_PR_PR13); // Сброс флага прерывания
    if (ExternInterruptTickCount >= DELAY_BUTTON_FILTER)
    {                // Выполнится, когда пройдёт 100 мс с момента обнуления данной переменной
        if (dTime - GlobalTickCount >= 2000)  LongBtnCount1++; // если разница между нажатиями больше 2секунд, то долгого нажатия
        else BtnCount1++; // Изменение состояния кнопки
        dTime = GlobalTickCount;
        ExternInterruptTickCount = 0; // Обнуление счётчика таймера
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