#include "task.h"
#include "handlers.h"

void task1(void){
    if(btn9_pressed){
        counter1 = (counter1 + 1) % 7;
        SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR3);
        SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR10);
        SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR11);
        SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12);
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
        SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);
        btn9_pressed=0;
    }
    if(btn8_pressed){
             led_periods[counter1] = (led_periods[counter1] + 1) % 4;
        btn8_pressed = 0;   
    }
    if(btn8_hold){
        // Каждое удержание меняет режим работы
        operation_mode = !operation_mode; // Инвертируем режим 
        btn8_hold = 0; // Сбрасываем флаг удержания
    }    
}

void task2(void){
    if(counter1 == 0) return;

    // Берем частоту для текущего светодиода
    uint8_t freq = led_periods[counter1];
    
    // Определяем период мигания
    uint16_t period = 0;
    // Определяем период мигания только для режима мерцания
    if(operation_mode == 0) { // Режим мерцания
        switch(freq) {
            case 0: period = 0; break;      // Выкл
            case 1: period = 2500; break;   //  0,2 Гц
            case 2: period = 625; break;    // 0,8 Гц
            case 3: period = 385; break;    // 1,3 Гц
        }
    } else { // Режим свечения - не мигаем
        period = 0;
    }

    if(operation_mode == 0) { 
    // Мигание
    if(period > 0) {
        if(tick_count - last_blink_time[counter1] >= period) {
            last_blink_time[counter1] = tick_count;
            led_state[counter1] = !led_state[counter1];
            
            // Переключаем текущий светодиод
            switch(counter1) {
                case 1: 
                    if(led_state[counter1]) SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR3);
                    else SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS3);
                    break;
                case 2:
                    if(led_state[counter1]) SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR10);
                    else SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10);
                    break;
                case 3:
                    if(led_state[counter1]) SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR11);
                    else SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11);
                    break;
                case 4:
                    if(led_state[counter1]) SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12);
                    else SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
                    break;
                case 5:
                    if(led_state[counter1]) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                    else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                    break;
                case 6:
                    if(led_state[counter1]) SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);
                    else SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
                    break;
            }
        }
    } else 
    {
          led_state[counter1] = 0;
        switch(counter1) {
            case 1: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS3); break;
            case 2: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10); break;
            case 3: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11); break;
            case 4: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12); break;
            case 5: SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); break;
            case 6: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2); break;
        }
    }
}
else {
         led_state[counter1] = 0;
        switch(counter1) {
            case 1: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS3); break;
            case 2: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS10); break;
            case 3: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS11); break;
            case 4: SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12); break;
            case 5: SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); break;
            case 6: SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2); break;
        }
    }

}