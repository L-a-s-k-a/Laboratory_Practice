#include "main.h"

int main(void) {
    //Версия через CMSIS
    //GPIO_Init_CMSIS();
    //do_with_CMSIS();

    //Версия через макрос
    //GPIO_Init_without_CMSIS_and_macros();
    //do_without_CMSIS_and_macros();

    //Версия без CMSIS и макросов
    //GPIO_Init_macros();
    //do_with_macros();

    //Дополнительное задание
    GPIO_Init_dop();
    green_lights();
}