#include "../Inc/init_lab2.h"
#include "Lab2.h"

uint8_t led_en = 0;                                         // Переменная хранящая сколько светодиодов включено
uint8_t num_led_blink = 0;                                  // Переменная хранящая номер светодиода, частота которого настраивается
uint32_t GlobalTickCount = 0;                               // Переменная таймер
uint8_t blink[6][2] = {0};                                  // Матрица для хранения номера диапазона частоты и номера самой частоты мигания лампочек
// uint8_t blink_1_1;                                          // Переменные для вывода в MCUViewer массива данных
// uint8_t blink_1_2;
// uint8_t blink_2_1;
// uint8_t blink_2_2;
// uint8_t blink_3_1;
// uint8_t blink_3_2;
// uint8_t blink_4_1;
// uint8_t blink_4_2;
// uint8_t blink_5_1;
// uint8_t blink_5_2;
// uint8_t blink_6_1;
// uint8_t blink_6_2;

int main(void)
{
    RCC_init_clocking();                                    // Настройка тактирования микроконтроллера
    GPIO_Init_Led_and_button();                             // Настройка портов GPIO (включение шин и настрока портов светодиодов)
    ITR_init();                                             // Настройка прерывания по кнопкам (PC12 и PC13)
    SysTick_Init();                                         // Настройка системного таймера

    while (1)
    {
       blinking_func(&led_en, blink, &GlobalTickCount);     // Основная функция, выполняющая задание согласно варианту 6
    //    blink_1_1 = blink[0][0];                             // Переменные для вывода в MCUViewer массива данных
    //    blink_1_2 = blink[0][1];
    //    blink_2_1 = blink[1][0];
    //    blink_2_2 = blink[1][1];
    //    blink_3_1 = blink[2][0];
    //    blink_3_2 = blink[2][1];
    //    blink_4_1 = blink[3][0];
    //    blink_4_2 = blink[3][1];
    //    blink_5_1 = blink[4][0];
    //    blink_5_2 = blink[4][1];
    //    blink_6_1 = blink[5][0];
    //    blink_6_2 = blink[5][1];
    }
}
