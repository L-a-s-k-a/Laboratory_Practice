#include "handlers.h"

void TIM1_UP_TIM10_IRQHandler(void){
    if (TIM10->SR & TIM_SR_UIF){       // Проверка флага обновления
        TIM10->SR &= ~TIM_SR_UIF;      // Сброс флага
        tick_count++; // Увеличиваем счетчик каждую 1 мс
        if(tick_count==1000)
        tick_count=0;
       
    }
}
void EXTI9_5_IRQHandler(void){
  
 if (EXTI->PR & EXTI_PR_PR8) {
        EXTI->PR = EXTI_PR_PR8;
        uint32_t now = tick_count;

        // Если кнопка нажата
        if (!(GPIOB->IDR & GPIO_IDR_ID8)) {
            btn8_press_time = now;
        }
        else {
            // Кнопка отпущено
            if ((now - btn8_press_time) >=2) {
                btn8_pressed = 1;
            }

            btn8_press_time = 0;
        }
    }
      if (EXTI->PR & EXTI_PR_PR9) {
        EXTI->PR = EXTI_PR_PR9;
        uint32_t now = tick_count;

        // Если кнопка нажата
        if (!(GPIOB->IDR & GPIO_IDR_ID9)) {
            btn9_press_time = now;
        }
        else {
            // Кнопка отпущено
            if ((now - btn9_press_time) >=2) {
                btn9_pressed = 1;
            }

            btn9_press_time = 0;
        }
    }
}
