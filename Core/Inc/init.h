#include <stdint.h>
#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F411xE/stm32f411xe.h"
#define RCC_GPIOС_TAKT (*(uint32_t *)(0x40023800UL + 0x30UL)) //Найстрока тактирования шины RCC
#define RCC_GPIOC_TAKT_EN 0x4UL //Значение необходимое для тактировования порта GPIOC шины RCC 
#define GPIOC_MODE (*(uint32_t *)(0x40020800UL + 0x00UL)) //Настройка работы порта PC8
#define GPIOC8_MODE_OUTPUT 0x10000UL //Значения необходимое для установки порта PC8 в режим выхода 
#define GPIOC8_PUSH_PULL_WORKING_MODE (*(uint32_t *)(0x40020800UL + 0x04UL)) // Настройка Push-Pull регистра порта GPIOD
#define OFF 0x00UL //Обнуление регистра
#define GPIOC8_SPEED (*(uint32_t *)(0x40020800UL + 0x08UL)) //Настройка скорости работы порта GPIOC 
#define GPIOC8_SPEED_MIDDLE 0x10000UL //Значение необходимое для задания средней скорости PC8
#define GPIOС8_PUSH_PULL_OFF (*(uint32_t *)(0x40020800UL + 0x0CUL)) //Настройка PU/PD резистора порта GPIOC
void GPIO_init(void);//Сигнатура функции настройки работы портов
void GLAV_ZADANIE(uint8_t *led, uint8_t *un_led, uint8_t *button);//Сигнатура функции главного задания
void DOP_ZADANIE(uint8_t *led, uint8_t *un_led, uint8_t *press);//Сигнатура функции дополнительного задания