#include "init.h"

void GPIO_Ini(void)
{
    // инициализация С порта через прямого обрашения к памяти
    *(uint32_t*)(0x40023800UL+0x30UL)|= 0x04UL;// включаем тактирование на  С  порте 
    *(uint32_t *)(0x40020800UL + 0x00UL) |= 0x1000000UL; // C moder1 12 cvetadiod output
    *(uint32_t *)(0x40020800UL + 0x08UL) |= 0x1000000UL; //С7 порт на средную скороть
    *(uint32_t *)(0x40020800UL + 0x0CUL) |= 0x00UL; //С7 порт

    // инициализация В порта через макросы
    SET_bit(RCC_GPIO_en, RCC_GPIOB_en); // включаем тактирование на  B  порте
    SET_bit(GPIOB_moder, GPIOB_moder_PIN7);    // B moder7 01 cvetadiod output 
    SET_bit(GPIOB_ospeed, GPIOB_ospeed_PIN7);  //B7порт на средную скороть
    SET_bit(GPIOB_pull_up_down, GPIOB_pull_up_down_PIN7); //B7 порт 0 

    // инициализация D порта через CMSIS
   SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // включаем тактирование на  D  порте через CMSIS
   SET_BIT(GPIOD->MODER, GPIO_MODER_MODE2_0);  // D moder2 01 cvetadiod output
   SET_BIT(GPIOB->OSPEEDR, GPIO_OSPEEDER_OSPEEDR2_0); //D2 порт на средную скороть
   SET_BIT(GPIOB->PUPDR, GPIO_PUPDR_PUPD2_0); //D2 порт 0
}



// void dop_test(uint8_t *counter1, uint8_t *button1, uint8_t *prev_state3, uint32_t *fortime2)
// {
//     if (READ_GPIO_B9 == 0)
//     {
//         for (volatile int i = 0; i < 1300; i++)
//             ;
//         if (READ_GPIO_B9 == 0 && *prev_state3 == 0)
//         {
//             *counter1 = (*counter1 + 1) % 4;
//             *button1 = 0;
//             *prev_state3 = 1;
//         }
//     }
//     else
//     {
//         *prev_state3 = 0;
//     }

//     if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0)
//     {
//         for (volatile int i = 0; i < 1300; i++);
//         if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0)
//         {
//             while (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0)
//             {
//                 (*fortime2)++;
//             }
//             if (*fortime2 < 300000)
//             {
//                 *button1 = (*button1 + 1) % 4;
//              }
//             if (*fortime2 >= 300000)
//             {
//                 *counter1 = (*counter1 + 1) % 4;
//                 *button1 = 0;
//             }
//             *fortime2 = 0;
//         }
//     }
  
// }

void main_task_pin_PB8_9(uint8_t *counter, uint8_t *button, uint8_t *prev_state1, uint8_t *prev_state2)
{
if (READ_GPIO_B9 == 0)
        {
            for (volatile int i = 0; i < 2000; i++);
            if (READ_GPIO_B9 == 0 && *prev_state2 == 0)
            {
                (*counter)++;
                RESET_GPIO_C12;
                RESET_GPIO_D2;
                RESET_GPIO_B7;
                if (*counter == 4)
                {
                    *counter = 1;
                }
                *prev_state2 = 1;
            }
        }
        else
        {
            *prev_state2 = 0;
        }
        if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0)
        {
            for (volatile int i = 0; i < 2000; i++);
            if (READ_BIT(GPIOB->IDR, GPIO_IDR_IDR_8) == 0 && *prev_state1 == 0)
            {
                RESET_GPIO_C12;
                RESET_GPIO_D2;
                RESET_GPIO_B7;
                *button = (*button + 1);
                if (*button == 4)
                {
                    *button = 1;
                }
                 if (*button == 3 && *counter == 3)
                {
                    *button = 1;
                }
                
            *prev_state1 = 1;
        }
    }
        else
        {
            *prev_state1 = 0;
        }
}
