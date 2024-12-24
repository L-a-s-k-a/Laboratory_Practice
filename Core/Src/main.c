#include "../Inc/init.h"
#include "../Inc/it_handlers.h"
 
#define FLICKER_PERIOD 400 //Период мерцания светодиода 
#define FLICKER_SEMIPERIOD 200 //Период мерцания светодиода 
#define WAIT_TIME 2000*10000 //Период мерцания светодиода 

void LedVal_Init();
uint16_t GlobalTickCount = 0, DelayTickCount = 0;
uint16_t GlobalTickBut1 = 0, GlobalTickBut2 = 0;

//float icount = 0.0;
uint8_t LedState; 
//uint32_t t1, t2, t3, t4, t5, t6;


uint32_t Ledfreq[3][3] = {{0.4, 1.3, 2},{0.6, 1.6, 2.2},{0.8, 1.9, 2.5}};
//uint32_t LedSetLoad[3][3];;
const uint32_t LedSetLoad[3][3] = {{12500000, 38461538, 2500000},{16666667, 6250000, 4545455},{12500000, 52631579, 4000000}}; //исправить
uint32_t LedLoad[6] = {0, 0, 0, 0, 0, 0};
uint32_t LedCount[6] = {0, 0, 0, 0, 0, 0};
uint8_t Ledflag[2][6] = {{1, 1, 1, 1, 1, 1},{1, 1, 1, 1, 1, 1}};
uint16_t Ledoffset[6] = {0, 7, 8, 9, 10, 14};
uint8_t CurrentLed = 0;
uint8_t Numb = 0;
uint8_t counterbut1 = 0;
uint8_t counterbut2 = 0;
uint8_t flagbut1 = 0;
uint8_t flagbut2 = 0;

void Led_light();

int main(void) 
{ 
    GPIO_Init_Self();
    GPIO_Init_CMSIS();
    RCC_Init(); // Инициализация тактирования системы 
    ITR_Init();// Инициализация прерываний  
    SysTick_Init();
    LedVal_Init();
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
                LedCount[i] = 0;
                if (Ledflag[2][i] == 1)
                {         
                    Ledflag[2][i] = 0;
                }
                else
                {
                    Ledflag[2][i] = 1;
                }
            }
        }
        Numb = counterbut1%7;
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
            counterbut1 = 0;
        }
        Led_light();

        
        //
        /*
        if(READ_BIT_Self(GPIOC_IDR, GPIOC_IDR_PIN6) != 0){
            if(flagbut1 == 0){
                flagbut1 = 1;
                counterbut1++;
                mydelay(20);
            }
            if(GlobalTickBut1 >= WAIT_TIME){ //Если прошло 2 секунды 
                LedLoad[CurrentLed] = LedSetLoad[counterbut2 / 3][counterbut2 % 3];
            } 
            else{ 
                ////////////////////////////////////////////////// кратковременное
            }                 
        }
        else if(flagbut1 == 1){
            flagbut1 = 0;
        }

        if(READ_BIT(GPIOC->IDR, GPIOC_IDR_PIN13) != 0){
            if(flagbut2 == 0)
            {
                GlobalTickBut2 = 0;    ///////////////////////////////////
                flagbut2 = 1;
                mydelay(20);
            }
            if(GlobalTickBut2 >= WAIT_TIME){ //Если прошло 2 секунды 
                counterbut2++;
            } 
            else{ 
                ////////////////////////////////////////////////// кратковременное
            }      
        }
        else if(flagbut2 == 1){
            flagbut2 = 0;
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
        
        /*
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BS7); //Включаем светодиод 
        mydelay(FLICKER_SEMIPERIOD); //Ожидаем 1 секунду 
        SET_BIT(GPIOB->BSRR, GPIO_BSRR_BR7); //Выключаем светодиод 
        mydelay(FLICKER_SEMIPERIOD); //Ожидаем 1 секунду 
        */
        
    } 
} 

void Led_light()
{
    uint32_t ODR_clear = 0;
    uint32_t ODR_set = 0;
    for (uint8_t i = 0; i < 6; i++){
        if ((Ledflag[1][i] == 1) && (Ledflag[2][i] == 1)){
            ODR_set = ODR_set + (1<<(Ledoffset[i]));
        }
        else{
            ODR_clear = ODR_clear + (1<<(Ledoffset[i]));
        }
    }

    //MODIFY_REG(GPIOB->ODR,ODR_clear,ODR_set);
    MODIFY_REG(GPIOB_ODR,ODR_clear,ODR_set);
}

void LedVal_Init()
{
    //Numtimes = freqAHB / ((SysTick_LOAD+1)*2*freq) (2 так как включение и выключение)
    /*
    float Ntkoeff = 180000000/(18*2);
    for (uint8_t i = 0; i < 3; i++){
        for (uint8_t j = 0; j < 3; i++){
            float a = round(Ntkoeff/Ledfreq[i][j])
            LedSetLoad[i][j] = (uint32_t)(a);
        }
    }
    */
    for (uint8_t i = 0; i < 6; i++){
        LedLoad[i] = LedSetLoad[0][0];
    }
}