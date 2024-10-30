#include "init.h"
void GPIO_Ini(void){
 RCC_GPIO_EN |= RCC_GPIOB_EN + RCC_GPIOC_EN; //Включение
 GPIOB_MODER |= GPIOB_MODE_PIN7_OUT; //Настройка работы 7-го пина
 GPIOB_OTYPER |= GPIOB_OTYPE_PIN7_PP; //Настройка на Push-Pull
 GPIOB_OSPEEDR |= GPIOB_OSPEED_PIN7_MID; //Настройка скорости
 GPIOB_PUPDR |= GPIOB_PUPDR_PIN7_NOPUPD; //Отключение PU/PD
}