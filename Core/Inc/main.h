

#ifndef CORE_INC_MAIN_H_
#define CORE_INC_MAIN_H_


#include "init.h"
//Задержка для кнопок
#define Button_Delay 300

	__attribute__((weak)) void _close(void){}
	__attribute__((weak)) void _lseek(void){}
	__attribute__((weak)) void _read(void){}
	__attribute__((weak)) void _write(void){}


#endif /* CORE_INC_MAIN_H_ */
