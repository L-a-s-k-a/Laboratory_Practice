
#include "it_handlers.h"
uint16_t  DelayTickCount;
uint32_t ExternInterruptTickCount;
extern uint8_t flag_led;
extern uint32_t systick_counter;


void update_led_state(void) {
    GPIOB->BSRR = LED1_PIN << 16 | LED2_PIN << 16 | LED3_PIN << 16 |
                  LED4_PIN << 16 | LED5_PIN << 16 | LED6_PIN << 16;
    switch (flag_led) {
        case 0: SET_BIT(GPIOB->BSRR,LED1_PIN); break;
        case 1: SET_BIT(GPIOB->BSRR,LED2_PIN); break;
        case 2: SET_BIT(GPIOB->BSRR,LED3_PIN); break;
        case 3: SET_BIT(GPIOB->BSRR,LED4_PIN); break;
        case 4: SET_BIT(GPIOB->BSRR,LED5_PIN); break;
        case 5: SET_BIT(GPIOB->BSRR,LED6_PIN); break;
    }
}


void SysTick_Handler(void) {
    systick_counter++; // 1ms 计时
    DelayTickCount++;
    ExternInterruptTickCount++;
}
void EXTI15_10_IRQHandler(void) {

    SET_BIT(EXTI->PR, EXTI_PR_PR13); 
    if(ExternInterruptTickCount >= DELAY_BUTTON_FILTER){ 
        flag_led = (flag_led + 1) % 6; // 切换到下一个 LED
        update_led_state();
        ExternInterruptTickCount = 0;
    }   
}


