#include "it_handlers.h"

extern uint16_t GlobalTickCount;

void SysTick_Handler(void) {
    ++GlobalTickCount;
}