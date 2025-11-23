#include "../Inc/init.h"
#include "../Inc/interrupt.h"

uint16_t ButtonTicks = 0;

uint8_t LED_OPT = 0;
uint8_t LED_NUM = 6;
uint8_t LED_OPTION[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};

bool LedState = false;

int main(void)
{
    GPIO_INIT();
    RCC_INIT();
    IQR_INIT();
    SYSTIMER_INIT();

    while (1)
    {
        working_mode(LED_NUM, LED_OPTION);
    }
}
