#include "E:\DMPT\Laboratory_Practice\Core\Inc\init.h"
int main(void)
{
    GPIO_init();//Функция инициализации всех портов платы
    uint8_t led = 0; // Перемнная для счёта зажжённых лампочек
    uint8_t button = 0; // Переменная для контроля положения 3 кнопки, была ли она нажата или нет
    uint8_t un_led = 0; // Перемнная для счёта затухших лампочек
    uint8_t press = 0; // Переменная для счёта количества нажатий на 3 кнопку в доп задании
    while(1)
    {
        GLAV_ZADANIE( &led, &un_led, &button); // Основное задание
        //DOP_ZADANIE( &led, &un_led, &press); //Дополнительное задание
    }
}
