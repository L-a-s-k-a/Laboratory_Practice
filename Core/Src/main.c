#include "../Inc/init.h"
#include "../Inc/it_handlers.h"


uint16_t DelayTickCount = 0;
uint32_t GlobalTickBut1Wait = 0, GlobalTickBut2Wait = 0;
//float Ledfreq[3][3]           = {{0.4, 1.3, 2},{0.6, 1.6, 2.2},{0.8, 1.9, 2.5}};
//Numtimes = freqAHB / ((SysTick_LOAD+1)*2*freq) (2 так как включение и выключение)
uint16_t LedSetLoad[3][3] = {{1250, 384, 250},{833, 312, 227},{625, 263, 200}};
uint8_t LedCurrfreq[6][2] = {{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0}};
uint16_t LedLoad[6] = {0, 0, 0, 0, 0, 0};
uint16_t LedCount[6] = {0, 0, 0, 0, 0, 0};
uint8_t Ledflag[2][6] = {{1, 1, 1, 1, 1, 1},{0, 0, 0, 0, 0, 0}};
uint8_t Led1flag = 0;
uint8_t Led2flag = 0;
const uint8_t LedOffset[6] = {0U, 7U, 14U, 8U, 9U, 10U};
uint8_t CurrentState = 6;
uint8_t CurrentLed = 0;
uint8_t Numb = 0;
uint8_t counterbut1 = 0;
uint8_t counterbut2 = 0;
uint8_t flagbut1 = 0, flagbut2 = 0;
uint8_t flagbut1long = 0, flagbut2long = 0;

void LedVal_Init();
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
        for (uint8_t i = 0; i < 6; i++){
            if (LedCount[i] >= LedLoad[i])
            {
                LedCount[i] = 0;
                if (Ledflag[1][i] == 1)
                {         
                    Ledflag[1][i] = 0;
                }
                else
                {
                    Ledflag[1][i] = 1;
                }
                //
                Led1flag = Ledflag[1][0];
                Led2flag = Ledflag[1][1];
            }
            
        }
        Numb = CurrentState%7;
        if (Numb != 0)
        {
            for (uint8_t i = 0; i < Numb; i++){
                Ledflag[0][i] = 1;
            }
        }
        else
        {
            for (uint8_t i = 0; i < 6; i++)
            {
                Ledflag[0][i] = 0;
            }
            CurrentState = 0;
        }
        if(flagbut1long == 1){ 
            if (LedCurrfreq[CurrentLed][0] >= 3)
            {
                LedCurrfreq[CurrentLed][0] = 0;
            }
            LedLoad[CurrentLed%6] = LedSetLoad[LedCurrfreq[CurrentLed][1]][LedCurrfreq[CurrentLed][0]];
            //LedLoad[CurrentLed%6] = LedSetLoad[counterbut2][counterbut1 % 3];
            flagbut1long = 0;
        } 
        if(flagbut2long == 1){     
            if (LedCurrfreq[CurrentLed][1] >= 3)
            {
                LedCurrfreq[CurrentLed][1] = 0;
            }
            LedLoad[CurrentLed%6] = LedSetLoad[LedCurrfreq[CurrentLed][1]][LedCurrfreq[CurrentLed][0]];
            //LedLoad[CurrentLed%6] = LedSetLoad[counterbut2][counterbut1 % 3];
            flagbut2long = 0;
        }
        if(flagbut1 == 1 && READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) == 0  && GlobalTickBut1Wait >= 50){ 
            flagbut1 = 0;
        }  
        if(flagbut2 == 1 && READ_BIT(GPIOC->IDR, GPIO_IDR_ID13) == 0 && GlobalTickBut2Wait >= 50){ 
            flagbut2 = 0;
        }  
        Led_light();
    } 
} 

void Led_light()
{
    uint32_t ODR_clear = 0x0UL;
    uint32_t ODR_set = 0x0UL;
    for (uint8_t i = 0; i < 6; i++){
        if ((Ledflag[0][i] == 1) && (Ledflag[1][i] == 1)){
            ODR_set = ODR_set + (0x1UL<<(LedOffset[i]));
        }
        else{
            ODR_clear = ODR_clear + (0x1UL<<(LedOffset[i]));
        }
    }
    MODIFY_REG(GPIOB->ODR,ODR_clear,ODR_set);
}

void LedVal_Init()
{
    for (uint8_t i = 0; i < 6; i++){
        LedLoad[i] = LedSetLoad[LedCurrfreq[i][0]][LedCurrfreq[i][1]];
        LedCount[i] = LedLoad[i];
    }
}