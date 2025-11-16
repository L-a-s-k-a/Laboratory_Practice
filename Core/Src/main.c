// Необходимо выбрать нужную конфигурацию в файле init.h

#include "init.h"
#include "main.h"

int main(void) {
    GPIO_Init();
    
    // Изначально все светодиоды выключены
    clear_all_leds();
    
    while(1) {
        
        handle_buttons();
        
        simple_delay(1000);
    }
}
