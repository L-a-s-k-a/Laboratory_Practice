#include "it_handlers.h"

extern uint8_t current_led;

extern uint32_t systick_counter;
uint16_t  DelayTickCount;

uint32_t ExternInterruptTickCount;

// SysTick 中断处理
void SysTick_Handler(void) {
    systick_counter++; // 1ms 计时
    DelayTickCount++;
    ExternInterruptTickCount++;
}
// 按钮1（PC13）中断处理
void EXTI15_10_IRQHandler(void) {
     
    SET_BIT(EXTI->PR, EXTI_PR_PR13); 
    if(ExternInterruptTickCount >= DELAY_BUTTON_FILTER){ 
        current_led = (current_led + 1) % 6; // 切换到下一个 LED
        update_led_state();
        ExternInterruptTickCount = 0;
    }   
  
}
void update_led_state(void) {
    // 关闭所有 LED
    GPIOB->BSRR = LED1_PIN << 16 | LED2_PIN << 16 | LED3_PIN << 16 |
                  LED4_PIN << 16 | LED5_PIN << 16 | LED6_PIN << 16;

    // 点亮当前 LED
    switch (current_led) {
          case 0: SET_BIT(GPIOB->BSRR,LED1_PIN); break;
        case 1: SET_BIT(GPIOB->BSRR,LED2_PIN); break;
        case 2: SET_BIT(GPIOB->BSRR,LED3_PIN); break;
        case 3: SET_BIT(GPIOB->BSRR,LED4_PIN); break;
        case 4: SET_BIT(GPIOB->BSRR,LED5_PIN); break;
        case 5: SET_BIT(GPIOB->BSRR,LED6_PIN); break;
    }
}

