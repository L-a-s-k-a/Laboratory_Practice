// main.c
#include "init.h"

// Переменные для кнопки B8
int counter8 = -1;
uint8_t check_status8 = 0;

int main(void) {
    // Настройка светодиода и кнопок
    GPIO_Init_with_memory_for_led5();  // PA5
    GPIO_Init_with_CMSIS();            // PA10, PA11 и кнопки
    
    // Главный цикл программы
    while (1) {
        // Работа с кнопкой B8
        if (CMSIS_READ_BUTTON_B8() != 0) {
            delay(300000);  // небольшая задержка
            
            // Если кнопка нажата и еще не обрабатывалась
            if((CMSIS_READ_BUTTON_B8() != 0) && (check_status8 == 0)) {
                counter8 = (counter8 + 1) % 3;  // меняем режим
                check_status8 = 1;  // запоминаем, что нажали
            }
        } else { 
            check_status8 = 0;  // кнопку отпустили
        }
        
        // В зависимости от режима работаем по-разному
        switch (counter8) {
            case 0:
                // Режим 0:
                if (CMSIS_READ_BUTTON_B1() != 0) {
                    *ADDR_GPIOA_BSRR = (1UL << 5);  // PA5 вкл
                } else {
                    *ADDR_GPIOA_BSRR = (1UL << 21); // PA5 выкл
                }
                
                if (CMSIS_READ_BUTTON_B2() != 0) {
                    CMSIS_LED_PA10_ON();  // PA10 вкл
                } else {
                    CMSIS_LED_PA10_OFF(); // PA10 выкл
                }
                
                if (CMSIS_READ_BUTTON_B3() != 0) {
                    CMSIS_LED_PA11_ON();  // PA11 вкл
                } else {
                    CMSIS_LED_PA11_OFF(); // PA11 выкл
                }
                break;
                
            case 1:
                // Режим 1:
                if (CMSIS_READ_BUTTON_B1() != 0) {
                    CMSIS_LED_PA10_ON();
                } else {
                    CMSIS_LED_PA10_OFF();
                }
                
                if (CMSIS_READ_BUTTON_B2() != 0) {
                    CMSIS_LED_PA11_ON();
                } else {
                    CMSIS_LED_PA11_OFF();
                }
                
                if (CMSIS_READ_BUTTON_B3() != 0) {
                    *ADDR_GPIOA_BSRR = (1UL << 5);
                } else {
                    *ADDR_GPIOA_BSRR = (1UL << 21);
                }
                break;
                
            case 2:
                // Режим 2:
                if (CMSIS_READ_BUTTON_B1() != 0) {
                    CMSIS_LED_PA11_ON();
                } else {
                    CMSIS_LED_PA11_OFF();
                }
                
                if (CMSIS_READ_BUTTON_B2() != 0) {
                    *ADDR_GPIOA_BSRR = (1UL << 5);
                } else {
                    *ADDR_GPIOA_BSRR = (1UL << 21);
                }
                
                if (CMSIS_READ_BUTTON_B3() != 0) {
                    CMSIS_LED_PA10_ON();
                } else {
                    CMSIS_LED_PA10_OFF();
                }
                break;
                
            default:
                // Все светодиоды выключены
                *ADDR_GPIOA_BSRR = (1UL << 21);
                CMSIS_LED_PA10_OFF();
                CMSIS_LED_PA11_OFF();
                break;
        }
    }
    
    return 0;
}

