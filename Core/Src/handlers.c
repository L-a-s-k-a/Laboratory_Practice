#include "handlers.h"

void SysTick_Handler(void) 
{
    tick_count++; // Увеличиваем счетчик каждую 1 мс
}

void EXTI9_5_IRQHandler(void){
 uint32_t now = tick_count; 
  if (EXTI->PR & EXTI_PR_PR8) {
        EXTI->PR = EXTI_PR_PR8;

        // Если кнопка НАЖАТА (уровень = 0)
        if (!(GPIOB->IDR & GPIO_IDR_ID8)) {
            // Запоминаем время только один раз
            btn8_press_time = now;
        }
        else { 
            // Кнопка отпущена
            uint32_t hold = now - btn8_press_time;
            if ((now - btn8_press_time) >= 5) {   
            if (hold >= 2000) {
                btn8_hold = 1;
            } else if (hold >= 2) {
                btn8_pressed = 1;
            }
        }
            // сброс
            btn8_press_time = 0;
        }
    }

      if (EXTI->PR & EXTI_PR_PR9) {
        EXTI->PR = EXTI_PR_PR9;

        // Если кнопка нажата
        if (!(GPIOB->IDR & GPIO_IDR_ID9)) {
            btn9_press_time = now;
        }
        else {
            // Кнопка отпущено
            if ((now - btn9_press_time) >= 5) {
                btn9_pressed = 1;
            }

            btn9_press_time = 0;
        }
    }
}