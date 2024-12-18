#include "leds.h"
#include "it_handlers.h"

extern uint16_t FLICKER_PERIOD;
extern uint8_t LedState; 
extern uint8_t b1_clicks;
extern uint8_t b2_clicks;
extern uint8_t b3_clicks;
extern uint16_t delay;
uint8_t flag1 = 0;
uint8_t flag2 = 0;
uint8_t flag3 = 0;
extern uint16_t GlobalTickCount, ExternInterruptTickCount1, ExternInterruptTickCount2, ExternInterruptTickCount3;
//////////////////////////////////////////////
extern uint16_t LedFreq[2][3];

uint8_t led[] = {8, 9, 5, 12, 13, 15};
extern uint16_t LedState2Data[3][6];

uint8_t flagUp = 1;
extern uint8_t flagUp2;
extern int8_t i1;
extern int8_t i2;
extern int8_t i3;

void b1_clicked(){
    if (ExternInterruptTickCount1>= DELAY_BUTTON_FILTER)
    {
        if (flag1 == 1){
            flag1 = 0;
            b1_clicks++;
            
        }else{
            flag1 = 1;
            b1_clicks++;
        }
        ExternInterruptTickCount1 = 0; // Обнуление счётчика таймера
    }
}

void b2_clicked(){
    if (ExternInterruptTickCount2>= DELAY_BUTTON_FILTER)
    {
        if (flag2 == 1){
            flag2 = 0;
            b2_clicks++;
        }else{
            flag2 = 1;
            b2_clicks++;
        }
        ExternInterruptTickCount2 = 0; // Обнуление счётчика таймера
    }
}

void b3_clicked(){
    if (ExternInterruptTickCount3>= DELAY_BUTTON_FILTER)
    {
        if (flag3 == 1){
            flag3 = 0;
            b3_clicks++;
        }else{
            flag3 = 1;
            b3_clicks++;
        }
        ExternInterruptTickCount3 = 0; // Обнуление счётчика таймера
    }
}



void leds_state1(void)
{
    if(GlobalTickCount >= LedFreq[0][b2_clicks%3] && flagUp)
    {
        if(i1==3){
            flagUp=0;
            i1=2;
            return;
        }
        if (GlobalTickCount >= LedFreq[0][b2_clicks%3]){
                GlobalTickCount = 0;
        }
        for(;;){
            SET_BIT(GPIOB->BSRR, 1UL << led[i1]);
            SET_BIT(GPIOB->BSRR, 1UL << led[5-i1]);
            i1++;
            return;
        }
        return;
    }
    
    if(GlobalTickCount >= LedFreq[0][b2_clicks%3] && !flagUp)
    {
        if(i1<=-1){
            flagUp=1;
            i1=0;
            return;
        }
        if (GlobalTickCount >= LedFreq[0][b2_clicks%3]){
                GlobalTickCount = 0;
        }
        for(;;){
            
            SET_BIT(GPIOB->BSRR, 1UL << led[i1]   << 16U);
            SET_BIT(GPIOB->BSRR, 1UL << led[5-i1] << 16U);
            i1--;
            return;
        }
        return;
    }
}

void leds_state_down(void){
    for(uint16_t k=0;k<6;k++)SET_BIT(GPIOB->BSRR, 1UL << led[k] << 16U);
    i1=0;
    i2=0;
    b2_clicks = 0;
    b3_clicks = 0;

    flagUp2=1;
    for(uint8_t g=0;g<6;g++){
        LedState2Data[0][g] = 0;
        LedState2Data[1][g] = 2000;
        LedState2Data[2][g] = 0;
    }
}

void leds_state2_goUp(void){
    if(GlobalTickCount >= LedFreq[1][2] && flagUp2)
    {
        if(i2==6){
            flagUp2=0;
            return;
        }
        if (GlobalTickCount >= LedFreq[1][2]){
                GlobalTickCount = 0;
        }
        for(;;){
            SET_BIT(GPIOB->BSRR, 1UL << led[i2]);
            i2++;
            return;
        }
        return;
    }
}




uint8_t b2_check, p_b2_check = 0;
uint8_t b3_check, p_b3_check = 0;
uint8_t counter = 0;
void b3_change(void)
{
    b3_check = b3_clicks;
    if (b3_check != p_b3_check)
    {
        b2_check = b2_clicks;
        if (b2_check != p_b2_check)
        {
            LedState2Data[1][--b3_clicks % 6] = LedFreq[1][b2_clicks];
            b2_clicks = 0;
            b3_clicks = 0;
        }
        b2_check = p_b2_check;
    }
    p_b3_check = b3_check;
}

extern uint8_t state2_flag;
uint8_t tmp_counter=0;

void leds_state2(void)
{
    leds_state2_goUp();
    b3_change();   
    if (!flagUp2 && state2_flag)
    {
        for(uint8_t tmp=0;tmp<6;tmp++){
            if (LedState2Data[0][tmp] >= LedState2Data[1][tmp] && LedState2Data[2][tmp]==0){
                LedState2Data[0][tmp] = 0;
                LedState2Data[2][tmp] = 1;
                SET_BIT(GPIOB->BSRR, 1UL << led[tmp] << 16U);
            }
        }
        state2_flag = 0;
    }

    if (!flagUp2 && !state2_flag)
    {
        for(uint8_t tmp=0;tmp<6;tmp++){
            if (LedState2Data[0][tmp] >= LedState2Data[1][tmp] && LedState2Data[2][tmp]==1){
                LedState2Data[0][tmp] = 0;
                LedState2Data[2][tmp] = 0;
                SET_BIT(GPIOB->BSRR, 1UL << led[tmp]);
            }
        }
        state2_flag=1;
    }
}