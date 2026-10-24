#include <init.h>

void delay(volatile uint32_t time){
    while(time--){
        __NOP();
    }
}

int main()
{
    dop_zadanie();   
    __init_myself_def();
    __init_mem();
    __init_CMSIS();

    uint8_t press_count = 0;        // Счетчик нажатий
    uint8_t but_pb9_press = 0;      // Кнопка режимов
    uint8_t but_pc7_press = 0;      // Кнопка шага
    uint8_t current_led_stage = 0;  // Число горящих светодиодов 0 - все выкл, 1-3 - горят PA7 - PA5, 4 - ещё и PB8
    uint8_t mode = 0;               // Выбор режима

    while(1){
        // Кнопка PB9 (режим). Нажата = 0
        if (BIT_READ(GPIOB_IDR, GPIO_ID_9) == 0){
            if (!but_pb9_press){
                delay(10000);
                
                but_pb9_press = 1; // Запоминаем, что кнопка удерживается
                press_count++;

                if (press_count == 5){
                    press_count = 0;    // Обнуляем счетчик нажатий
                    mode = !mode;       // Меняем режим

                    if (mode == 1){       
                        // PC7 - светодиод 
                        SET_BIT(GPIOC -> MODER, GPIO_MODER_MODER7_0);  // Настраиваем как выход                                                                     
                        SET_BIT(GPIOC -> BSRR, GPIO_BSRR_BS7);         // Включили светодиод
                    }
                    else{
                        // PC7 - кнопка
                        SET_BIT(GPIOC -> BSRR, GPIO_BSRR_BR7);         // Гасим светодиод
                        CLEAR_BIT(GPIOC -> MODER, GPIO_MODER_MODER7);  // Настраиваем как вход
                    }            
                }
                
            }
        }
        else if (but_pb9_press){
            delay(10000);
            but_pb9_press = 0; // Кнопка отпущена
        }

        // кнопка PC7 (шаг). Нажата = 0
        if (READ_BIT(GPIOC -> IDR, GPIO_IDR_ID7) == 0){
            if (!but_pc7_press){
                delay(10000);
                but_pc7_press = 1; // Запомним, что кнопка удерживается

                if (mode == 0){
                    current_led_stage++;

                    if (current_led_stage == 1)      BIT_SET(GPIOA_BSRR, GPIO_BS_7);            // Включаем первый светодиод 
                    else if (current_led_stage == 2) SET_BIT(GPIOA -> BSRR, GPIO_BSRR_BS6);     // Включаем второй светодиод
                    else if (current_led_stage == 3) SET_BIT(GPIOA -> BSRR, GPIO_BSRR_BS5);     // Включаем третий светодиод
                    else { 
                        SET_BIT(GPIOA -> BSRR, GPIO_BSRR_BR7 | GPIO_BSRR_BR6 | GPIO_BSRR_BR5);  // Гасим и начинаем сначала
                        current_led_stage = 0;
                    }
                }
            }
        }
        else if (but_pc7_press){
            delay(10000);
            but_pc7_press = 0; // Кнопка отжата
        }
    }
}