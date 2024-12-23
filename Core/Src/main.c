#include "../Inc/init.h"
#include "../Inc/it_handlers.h"
 
#define FLICKER_PERIOD 400 //Период мерцания светодиода 
#define FLICKER_SEMIPERIOD 200 //Период мерцания светодиода 
 
uint16_t GlobalTickCount = 0, DelayTickCount = 0; 
float icount = 0.0;
uint8_t LedState; 
//uint32_t t1, t2, t3, t4, t5, t6;
 //freqAHB / ((SysTick_LOAD+1)*2*freq) (2 так как включение и выключение)
uint32_t Ledfreq[3][3] = {{0.4, 1.3, 2},{0.6, 1.6, 2.2},{0.8, 1.9, 2.5}};
uint32_t LedSetLoad[3][3];
for (uint8_t i = 0; i < 3; i++){
    for (uint8_t j = 0; j < 3; i++){
        LedSetLoad[i][j] = (uint32_t)round(Ledfreq[i][j]);
    }
}
//const uint32_t LedSetLoad[3][3] = {{12500000, 38461538, 2500000},{16666667, 6250000, 4545455},{12500000, 52631579, 4000000}};

uint32_t LedLoad[6] = {0, 0, 0, 0, 0, 0};
uint32_t LedCount[6] = {0, 0, 0, 0, 0, 0};
uint8_t Ledflag[2][6] = {{0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0}};
uint16_t Ledoffset[6] {0, 7, 8, 9, 10, 14};
uint8_t Numb = 0;
uint8_t counter = 0;

void Led_light();

int main(void) 
{ 
    GPIO_Init_Self();
    GPIO_Init_CMSIS();
    RCC_Init(); // Инициализация тактирования системы 
    ITR_init();// Инициализация прерываний  
    SysTick_Init();
       
    while (1) 
    {
        /*
        t1 = READ_REG(SysTick->VAL);
        t2 = READ_REG(SysTick->CTRL);
        t3 = READ_REG(SysTick->LOAD);
        icount = icount + 0.001;
        */
        for (uint8_t i = 0; i < 6; i++){
            if (LedCount[i] >= LedLoad[i])
            {
                LedCount[i];
                if (Ledflag[i] == 1)
                {
                    Ledflag[i] = 0;
                }
                else
                {
                    Ledflag[i] = 1;
                    
                }
            }
        }
        Numb = counter%7;
        if (Numb != 0)
        {
            Ledflag[1][Numb-1] = 1;
        }
        else
        {
            for (uint8_t i = 0; i < 6; i++)
            {
                Ledflag[1][i] = 0;
            }
            counter = 0;
        }
        Led_light();
        //
        /*
        if(READ_BIT_Self(GPIOC_IDR, GPIOC_IDR_PIN0) != 0){
            if(flag == 0){
                flag = 1;
                counter++;
            }           
        }
        else if(flag == 1){
            flag = 0;
        }
        */

        
        /*if(LedState) SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Если нажали один раз на кнопку светодиод включится 
        else SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Если нажали ещё раз светодиод выключится 
        */
 
        /*if(GlobalTickCount >= FLICKER_PERIOD / 2){ //Если прошло 1000 прерываний системного таймера = 1 секунда 
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Включаем светодиод 
            if(GlobalTickCount >= FLICKER_PERIOD){ //Если прошло 2000 прерываний системного таймера = 2 секунды 
                GlobalTickCount = 0; //Обнуляем переменную счётчика системного таймера 
            } 
        } 
        else{ 
            SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Иначе выключаем светодиод 
        } */
        
        
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Включаем светодиод 
        User_Delay(FLICKER_SEMIPERIOD); //Ожидаем 1 секунду 
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Выключаем светодиод 
        User_Delay(FLICKER_SEMIPERIOD); //Ожидаем 1 секунду 
        
    } 
} 

void Led_light()
{
    uint32_t ODR_clear = 0;
    uint32_t ODR_set = 0;
    for (uint8_t i = 0; i < 6; i++){
        if ((Ledflag[1][i] == 1) && (Ledflag[2][i] == 1)){
            ODR_set = ODR_set + (0x1UL<<Ledoffset[i]);
        }
        else{
            ODR_clear = ODR_clear + (0x1UL<<Ledoffset[i]);
        }
    }
    //MODIFY_REG(GPIOB->ODR,ODR_clear,ODR_set);
    MODIFY_REG(GPIOB_ODR,ODR_clear,ODR_set);
}