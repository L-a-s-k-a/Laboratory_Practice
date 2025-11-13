#include "init.h"
#include <stdint.h>

uint8_t counter = 1;
uint8_t button = 0;
uint8_t prev_state1 = 0;
uint8_t prev_state2 = 0;
int main(void)
{
    GPIO_Ini();
    while (1)
    {
    main_task_pin_PB8_9(&counter, &button, &prev_state1, &prev_state2);
    if( counter == 1){
                switch (button)
                {
                case 1:
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS_12);
                    break;
                case 2:
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS_7);
                    break;
                case 3:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS_2);
                    break;
                default:
                    break;
                } 
            }
            if( counter == 2){
               switch (button)
                {
                case 1:
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
                    break;
                case 2:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
                    break;
                case 3:
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                    break;
                default:
                    break;
                } 
            }
             if( counter == 3){
               switch (button)
                {
                case 1:
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS12);
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS2);
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7);
                    break;
                case 2:
                    SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR12);
                    SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR2);
                    SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7);
                    break;
                default:
                    break;
                }  
            }
    }
}

     // uint32_t fortime2 = 0; 
    // uint8_t prev_state3 = 0;
    // uint8_t counter1 = 0;
    // uint8_t button1 = 0;
    // uint32_t timeArr[4] = {0, 0, 0, 0};

//Доп задача: одна кнопка переключает светадиоды, второя  задает частоту мерцании (в трех различных частотах) в коротком нажатии, при долгом переключает на следуюший светадиод  
    // dop_test(&counter1, &button1, &prev_state3, &fortime2);
  
    //  switch (button1)
    //             {
    //             case 0:
    //                 timeArr[counter1] = 0;
    //                 break;
    //             case 1:
    //                 timeArr[counter1] = 30000;
    //                 break;
    //             case 2:
    //                 timeArr[counter1] = 70000;
    //                 break;
    //             case 3:
    //                 timeArr[counter1] = 150000;
    //                 break;
    //             }
    // fortime2 = 0;
    //   if (counter1 == 1)
    // {
    //     RESET_GPIO_C12;
    //     RESET_GPIO_D2;
    //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS_7);
    //     for (volatile int i = 0; i < timeArr[counter1]; i++)
    //     {
    //         if (READ_GPIO_B9 == 0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
    //             break;
    //     }
    //     SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR_7);
    //     for (volatile int i = 0; i < timeArr[counter1]; i++)
    //     {
    //         if (READ_GPIO_B9 == 0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
    //             break;
    //     }
    // }

    // else if (counter1 == 2)
    // {
    //     RESET_GPIO_B7;
    //     RESET_GPIO_D2;
    //     SET_BIT(GPIOC->BSRR, GPIO_BSRR_BS_12); // Включить
    //     for (volatile int i = 0; i < timeArr[counter1]; i++)
    //     {
    //         if (READ_GPIO_B9 == 0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
    //             break;
    //     }
    //     SET_BIT(GPIOC->BSRR, GPIO_BSRR_BR_12); // Выключить
    //     for (volatile int i = 0; i < timeArr[counter1]; i++)
    //     {
    //         if (READ_GPIO_B9 == 0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
    //             break;
    //     }
    // }
    // else if (counter1 == 3)
    // {
    //     RESET_GPIO_B7;
    //     RESET_GPIO_C12;
    //     SET_BIT(GPIOD->BSRR, GPIO_BSRR_BS_2); // Включить
    //     for (volatile int i = 0; i < timeArr[counter1]; i++)
    //     {
    //         if (READ_GPIO_B9 == 0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
    //             break;
    //     }
    //     SET_BIT(GPIOD->BSRR, GPIO_BSRR_BR_2); // Выключить
    //     for (volatile int i = 0; i < timeArr[counter1]; i++)
    //     {
    //         if (READ_GPIO_B9 == 0 && READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8))
    //             break;
    //     }
    // }