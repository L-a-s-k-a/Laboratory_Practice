#include "init.h"

int main(void) {
    //Функции для выполнения лабораторной работы без использования CMSIS и макросов
    GPIO_Ini_Classic();
    Task_Classic();

    //Функции для выполнения лабораторной работы с использованием макросов
    //GPIO_Ini_Macros();
    //Task_Macros();

    //Функции для выполнения лабораторной работы с использованием CMSIS
    //GPIO_Ini_CMSIS();
    //Task_CMSIS();
    
    //Доп. задание для лабораторной работы с использованием CMSIS
    //GPIO_Ini_CMSIS();
    //Extra_Task_CMSIS();
}

   