#include "init.h"

#define LED1_SET    BS_B0           // Uses GPIO_A
#define LED2_SET    BS_B1           // Uses GPIO_C
#define LED3_SET    BS_B2           // Uses GPIO_C
#define LED4_SET    BS_B3           // Uses GPIO_F
#define LED5_SET    BS_B4           // Uses GPIO_F
#define LED6_SET    BS_B5          // Uses GPIO_F

#define BUT1        IDR_B0      // Включает светодиоды поочередно
#define BUT2        IDR_B1      // Выключает светодиоды в обратном порядке
#define BUT3        IDR_B7      // Меняет местами функционал кнопок 1 и 2

int main(void){
    GPIO_init();

    uint8_t light_amount=0;     // Количество включенных диодов
    uint8_t is_reverse=0;       // Инвертированы ли функции кнопок 1 и 2

    while(1){
        uint8_t light_up_mode=0;    // режим: включаем (1)/выключаем (2) диоды или ничего не делаем (0)

        if(BIT_READ(GPIO_A+IDR,BUT3)){          //Проверка сигнала на GPIO_A IDR 7 - Меняет местами функционал кнопок 1 и 2
            if(is_reverse) is_reverse=0;
            else is_reverse=1;
        }
        else if(BIT_READ(GPIO_A+IDR,BUT1)){     //Проверка сигнала на GPIO_A IDR 5 - Включает светодиоды поочередно
            if(is_reverse) light_up_mode=2;
            else light_up_mode=1;
        }
        else if(BIT_READ(GPIO_A+IDR,BUT2)){     //Проверка сигнала на GPIO_A IDR 6 - Выключает светодиоды в обратном порядке
            if(is_reverse) light_up_mode=1;
            else light_up_mode=2;
        }

        if (light_up_mode>>1 & (light_amount>0)) light_amount--;
        else if (light_up_mode & (light_amount<6)) light_amount++;

        if (light_amount==1) {
            BIT_SET(GPIO_A+BSRR,LED1_SET);
            BIT_SET(GPIO_C+BSRR,BR_MUL*(LED2_SET+LED3_SET));
            BIT_SET(GPIO_F+BSRR,BR_MUL*(LED4_SET+LED5_SET+LED6_SET));
        }
        else if (light_amount==2) {
            BIT_SET(GPIO_A+BSRR,LED1_SET);
            BIT_SET(GPIO_C+BSRR,LED2_SET+BR_MUL*LED3_SET);
            BIT_SET(GPIO_F+BSRR,BR_MUL*(LED4_SET+LED5_SET+LED6_SET));
        }
        else if (light_amount==3) {
            BIT_SET(GPIO_A+BSRR,LED1_SET);
            BIT_SET(GPIO_C+BSRR,LED2_SET+LED3_SET);
            BIT_SET(GPIO_F+BSRR,BR_MUL*(LED4_SET+LED5_SET+LED6_SET));
        }
        else if (light_amount==4) {
            BIT_SET(GPIO_A+BSRR,LED1_SET);
            BIT_SET(GPIO_C+BSRR,LED2_SET+LED3_SET);
            BIT_SET(GPIO_F+BSRR,LED4_SET+BR_MUL*(LED5_SET+LED6_SET));
        }
        else if (light_amount==5) {
            BIT_SET(GPIO_A+BSRR,LED1_SET);
            BIT_SET(GPIO_C+BSRR,LED2_SET+LED3_SET);
            BIT_SET(GPIO_F+BSRR,LED4_SET+LED5_SET+BR_MUL*LED6_SET);
        }
        else if (light_amount==6) {
            BIT_SET(GPIO_A+BSRR,LED1_SET);
            BIT_SET(GPIO_C+BSRR,LED2_SET+LED3_SET);
            BIT_SET(GPIO_F+BSRR,LED4_SET+LED5_SET+LED6_SET);
        }
        else {
            BIT_SET(GPIO_A+BSRR,BR_MUL*LED1_SET);
            BIT_SET(GPIO_C+BSRR,BR_MUL*(LED2_SET+LED3_SET));
            BIT_SET(GPIO_F+BSRR,BR_MUL*(LED4_SET+LED5_SET+LED6_SET));
        }
    }
}