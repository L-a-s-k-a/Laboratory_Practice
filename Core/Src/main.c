#include "init.h"
#include "it_handlers.h"

uint16_t GlobalTickCount;

int main(void) {
    GPIO_Ini();
    RCC_Ini();
    ITR_Ini();
    while (1) {
        
    }
}