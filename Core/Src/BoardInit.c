#include "BoardInit.h"

void enableClockC(void) { ENABLE_GPIOC_CLOCK(); }
void enableClockD(void) { ENABLE_GPIOD_CLOCK(); }
void enableClockE(void) { ENABLE_GPIOE_CLOCK(); }