#include <stdint.h>
#include <../Inc/init.h>
int main(void) {
 *(uint32_t*)(0x40023800UL + 0x30UL) |= 0x02; //Включение
 *(uint32_t*)(0x40020400UL + 0x00UL) |= 0x4000; //Настройка работы
 *(uint32_t*)(0x40020400UL + 0x04UL) |= 0x00; //Настройка на PushPull работу 7-го пина GPIOB (Output Push-Pull)
 *(uint32_t*)(0x40020400UL + 0x08UL) |= 0x4000; //Настройка скорости
 *(uint32_t*)(0x40020400UL + 0x0CUL) |= 0x00; //Отключение PU/PD
 while(1){
    if((*(uint32_t*)(0x40020800UL + 0x10UL) & 0x2000UL) != 0){
    *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x80; //Установка
 }
 else{
 *(uint32_t*)(0x40020400UL + 0x18UL) |= 0x800000; //Установка
 }
 }
}