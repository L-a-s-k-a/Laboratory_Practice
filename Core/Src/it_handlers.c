#include "it_handlers.h"
// extern uint32_t led1_tick;
// extern uint32_t led2_tick;
// extern uint32_t led3_tick;
extern uint32_t led_tick[6];
uint16_t ExternInterruptTickCount;
extern uint8_t shortState;
extern uint8_t mode;
void SysTick_Handler(void){ // Системный таймер 1кГц
    ExternInterruptTickCount++;
    led_tick[0]++;
    led_tick[1]++;
    led_tick[2]++;
    led_tick[3]++;
    led_tick[4]++;
    led_tick[5]++;
}
uint8_t btnState = 0;
extern uint8_t BtnCount2;
void EXTI0_IRQHandler(void){
    SET_BIT(EXTI->PR, EXTI_PR_PR0); // завершение прерывания
    shortState = 0;
    if (ExternInterruptTickCount >= DELAY_BUTTON_FILTER){ // Выполнится, когда пройдёт 100 мс с момента обнуления данной переменной DELAY_BUTTON_FILTER
        if(btnState == 0){
            ExternInterruptTickCount = 0; // Обнуление счётчика таймерад
            btnState = 1;
        }
        else{ //(btnState == 1){
            btnState = 0;
            if(ExternInterruptTickCount > 2000){
                shortState = 1;
                if(ExternInterruptTickCount > 4000){
                    shortState = 0;
                    mode = !mode;
                } 
            }
            else BtnCount2++;
            ExternInterruptTickCount = 0;
        }
    }
}
    // if(BtnCount == 1){
    //     LedState = !LedState;
    //     BtnCount = 0;
    // }