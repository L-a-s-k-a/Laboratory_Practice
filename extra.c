#include "extra.h"
#include "init.h"

// Локальная задержка для этого модуля
static void extra_delay(volatile uint32_t time) {
    while(time--) {
        __NOP();
    }
}

// Реализация доп. задания
void run_extra_task(void) {

    const uint32_t pim_periods[6] = {25000, 15000, 3000, 1000, 150, 15};
    uint8_t step = 6; // Начальный шаг яркости

    uint8_t btn1_was_pressed = 0;
    uint8_t btn2_was_pressed = 0;


    while (1) {
        //  1 Увеличение яркости (Пин PA5)
        if ((GPIOA->IDR & GPIO_IDR_ID5) == 0) { 
            extra_delay(20000); // Антидребезг
            if ((GPIOA->IDR & GPIO_IDR_ID5) == 0 && !btn1_was_pressed) {
                if (step < 5) step++; // Максимальный шаг 
                btn1_was_pressed = 1;            
            }
        } else { btn1_was_pressed = 0; }

        // 2 Уменьшение яркости (Пин PA6)
        if ((GPIOA->IDR & GPIO_IDR_ID6) == 0) {
            extra_delay(20000); // Антидребезг
            if ((GPIOA->IDR & GPIO_IDR_ID6) == 0 && !btn2_was_pressed) {
                if (step > 0) step--; // Минимальный шаг
                btn2_was_pressed = 1;            
            }
        } else { btn2_was_pressed = 0; }

        //  ЧИМ СИГНАЛ
        if (step == 0) {
            // условие: на самом последнем шаге гаснем полностью
            GPIOB->BSRR = GPIO_BSRR_BR3; 
        } else {
            GPIOB->BSRR = GPIO_BSRR_BS3;    // Импульс ВКЛ
            extra_delay(2000);              // Время горения
            
            GPIOB->BSRR = GPIO_BSRR_BR3;    // Импульс ВЫКЛ
            extra_delay(pim_periods[step]); // Динамическая пауза из таблицы 
        }
    }
}
