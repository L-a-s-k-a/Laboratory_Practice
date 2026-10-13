#ifndef INIT_H_ // защита от повтороного подключения
#define INIT_H_

#include <stdint.h>
#include "stm32f446xx.h"

#define BIT_SET(REG, VAL)   ((REG) |= (VAL))
#define BIT_READ(REG, VAL)  ((REG) & (VAL))

// конфигурация порта А
#define MY_GPIOA_BASE       0x40020000UL // адрес начала Порта А в памяти контроллера
#define MY_GPIOA_MODER      (*(uint32_t*)(MY_GPIOA_BASE + 0x00UL))  //адрес базы + смещение регистра режимов
#define MY_GPIOA_INPUT_5_6  ((0x3UL << 10) | (0x3UL << 12)) // битовая маска

void _init_mem();
void _init_myself_def();
void _init_CMSIS();

#endif /* INIT_H_ */
