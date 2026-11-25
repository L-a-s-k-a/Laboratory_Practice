#include "init.h"

void GPIO_init(){
    /*
    // Настраиваем  STM32F429ZI
    BIT_SET(RCC+AHB1ENR,RCC_GPIO_A_EN+RCC_GPIO_C_EN+RCC_GPIO_F_EN); // Активируем тактирование портов A, C, F по RCC AHB1ENR
    // Настраиваем  GPIO_A
    BIT_SET(GPIO_A+MODER,0x00000040UL);                             //Режим General Purpose Output для 3го пина
    BIT_SET(GPIO_A+OTYPER,0x00UL);                                  //Режим вывода Push-Pull
    BIT_SET(GPIO_A+OSPEEDR,0x00000040UL);                           //Режим скорости работы 3го пина: middle
    BIT_SET(GPIO_A+PUPDR,0x00UL);                                   //Оставляем пины без подтяжки и стяжки к общему питанию
    // Настраиваем  GPIO_C
    BIT_SET(GPIO_C+MODER,0x00000041UL);                             //Режим General Purpose Output для 0, 3 пинов
    BIT_SET(GPIO_C+OTYPER,0x00UL);                                  //Режим вывода Push-Pull
    BIT_SET(GPIO_C+OSPEEDR,0x00000041UL);                           //Режим скорости работы 0, 3 пинов: middle
    BIT_SET(GPIO_C+PUPDR,0x00UL);                                   //Оставляем пины без подтяжки и стяжки к общему питанию
    // Настраиваем  GPIO_F
    BIT_SET(GPIO_F+MODER,0x00100440UL);                             //Режим General Purpose Output для 3, 5, 10 пинов
    BIT_SET(GPIO_F+OTYPER,0x00UL);                                  //Режим вывода Push-Pull
    BIT_SET(GPIO_F+OSPEEDR,0x00100440UL);                           //Режим скорости работы 3, 5, 10 пинов: middle
    BIT_SET(GPIO_F+PUPDR,0x00UL);                                   //Оставляем пины без подтяжки и стяжки к общему питанию
    */
    // Настраиваем  STM32F429ZI
    BIT_SET(RCC+AHB1ENR,RCC_GPIO_A_EN+RCC_GPIO_B_EN);               // Активируем тактирование портов A, C, F по RCC AHB1ENR
    // Настраиваем  GPIO_A
    BIT_SET(GPIO_A+MODER,0x0000003FUL);                             //Режим General Purpose Output для 3го пина
    BIT_SET(GPIO_A+OTYPER,0x00UL);                                  //Режим вывода Push-Pull
    BIT_SET(GPIO_A+OSPEEDR,0x0000003FUL);                           //Режим скорости работы 3го пина: middle
    BIT_SET(GPIO_A+PUPDR,0x00UL);                                   //Оставляем пины без подтяжки и стяжки к общему питанию
    // Настраиваем  GPIO_B
    BIT_SET(GPIO_B+MODER,0x00000007UL);                             //Режим General Purpose Output для 0, 1, 2 пинов
    BIT_SET(GPIO_B+OTYPER,0x00UL);                                  //Режим вывода Push-Pull
    BIT_SET(GPIO_B+OSPEEDR,0x00000007UL);                           //Режим скорости работы 0, 1, 2 пинов: middle
    BIT_SET(GPIO_B+PUPDR,0x00UL);  
}