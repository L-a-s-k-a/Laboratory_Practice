#include "it_handlers.h"

volatile uint32_t GlobalTickCount = 0;

void SysTick_Handler(void) {
    GlobalTickCount++;
}