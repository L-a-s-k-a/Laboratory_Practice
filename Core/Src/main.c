#include "../Inc/init.h"
#include "../Inc/it_handlers.h"
 
#define FLICKER_PERIOD 400 //Период мерцания светодиода 
#define FLICKER_SEMIPERIOD 200 //Период мерцания светодиода 
 
uint16_t GlobalTickCount = 0, DelayTickCount = 0; 
float icount = 0.0;
uint8_t LedState; 
uint32_t t1, t2, t3, t4, t5, t6;

 //Косяк что не поделил частоту (умножил так как включение и выключение)
const uint32_t LedSetLoad[3][3] = {{25000000, 7692308, 5000000},{16666667, 6250000, 4545455},{12500000, 52631579, 4000000}};
uint32_t LedLoad[6] = {0, 0, 0, 0, 0, 0};
uint32_t LedCount[6] = {0, 0, 0, 0, 0, 0};
uint8_t Ledflag[6] = {0, 0, 0, 0, 0, 0};

void Led_light()
{

}

int main(void) 
{
    
    GPIO_Init_Self();
    GPIO_Init_CMSIS();
    RCC_Init(); // Инициализация тактирования системы 
    ITR_init();// Инициализация прерываний  
    SysTick_Init();
       
    while (1) 
    {
        
        t1 = READ_REG(SysTick->VAL);
        t2 = READ_REG(SysTick->CTRL);
        t3 = READ_REG(SysTick->LOAD);
        icount = icount + 0.001;
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
        Led_light();
        

        
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
