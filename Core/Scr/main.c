#include "../Inc/init.h"

uint8_t but_check = 0;
uint8_t pusher = 0;

int main(void)
{
    GPIO_INIT();
    while (1)
    {
        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) == RESET) && (pusher == 0))
        {
            pusher = time_skeeper();
        }
        if ((READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) != RESET) && (pusher == 1))
        {
            but_check = diode_mode(pusher, but_check);
            pusher = 0;
        }
        main_process(but_check);
    }
}
