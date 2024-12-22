#include "it_handlers.h"

extern uint8_t current_led;
extern uint8_t  work_mode;
extern uint8_t blink_frequency;
extern uint8_t button2_pressed; 
extern uint32_t button2_timer;
extern uint32_t systick_counter;
uint16_t  DelayTickCount;
extern const uint32_t led_pins[];
extern const uint32_t blink_periods[];
extern uint8_t flagbutton1;
volatile uint8_t button2_handled = 0;

// SysTick 中断处理
void SysTick_Handler(void) {
    systick_counter++; // 1ms 计时
    DelayTickCount++;
}


// 按钮1（PC13）中断处理
void EXTI15_10_IRQHandler(void) {
    flagbutton1=!flagbutton1;
     if (EXTI->PR & EXTI_PR_PR13) {
        SET_BIT(EXTI->PR, EXTI_PR_PR13); 
     }
       
    current_led = (current_led + 1) % 6; // 切换到下一个 LED
    User_Delay(DELAY_BUTTON_FILTER);
    update_led_state();

    }


void update_led_state(void) {
    // 关闭所有 LED
    GPIOB->BSRR = LED1_PIN << 16 | LED2_PIN << 16 | LED3_PIN << 16 |
                  LED4_PIN << 16 | LED5_PIN << 16 | LED6_PIN << 16;

    // 点亮当前 LED
    switch (current_led) {
        case 0: GPIOB->BSRR = LED1_PIN; break;
        case 1: GPIOB->BSRR = LED2_PIN; break;
        case 2: GPIOB->BSRR = LED3_PIN; break;
        case 3: GPIOB->BSRR = LED4_PIN; break;
        case 4: GPIOB->BSRR = LED5_PIN; break;
        case 5: GPIOB->BSRR = LED6_PIN; break;
    }
}



void User_Delay(uint32_t delay){

    while(DelayTickCount < delay){} //Цикл, благодаря которому происходит задержка программы
    if(DelayTickCount >= delay) DelayTickCount = 0; //Обнуление переменной счётчика, при достижении заданного пользователем значения

}


