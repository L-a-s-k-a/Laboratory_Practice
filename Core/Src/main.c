#include "init.h"

int main(void){
GPIO_Ini();
while(1){
    if(READ_GPIO_C13 != 0){
        SET_GPIO_B7; //Установка единицы в 7-ой бит регистра ODR
        }
        else{
        RESET_GPIO_B7; //Установка нуля в 7-ой бит регистра ODR
        }
    }
}