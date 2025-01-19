#include "it_handlers.h"
#include <stdint.h>

// 来自 main.c
extern volatile uint32_t GlobalTickCount;
extern uint8_t  LedOn[6];
extern uint8_t  NumLedOn;
extern uint8_t  FreqIndex;
extern uint32_t BlinkPeriods[];
extern uint32_t CurrentPeriod;

typedef enum {
    MODE_BLINK=0,
    MODE_CONSTANT
} WorkMode_t;
extern volatile WorkMode_t CurrentMode;

// 按键标记
volatile uint8_t  BtnPressed = 0;
volatile uint32_t PressStart  = 0;

void SysTick_Handler(void)
{
    GlobalTickCount++;
}

void EXTI15_10_IRQHandler(void)
{
    if(EXTI->PR & (1 << 13))
    {
        EXTI->PR |= (1 << 13);

        uint8_t level = (GPIOC->IDR & (1<<13)) ? 1 : 0; 
        static uint32_t lastIRQ=0;
        if((GlobalTickCount - lastIRQ)<DELAY_BUTTON_FILTER)
            return;
        lastIRQ=GlobalTickCount;

        if(level==1)
        {
            // 按下
            BtnPressed=1;
            PressStart=GlobalTickCount;
        }
        else
        {
            // 松开
            BtnPressed=0;
            uint32_t pressTime = GlobalTickCount - PressStart;

            if(pressTime < 2000)
            {
                // 短按
                if(NumLedOn<6)
                {
                    LedOn[NumLedOn] = 1;
                    NumLedOn++;
                }
                else
                {
                    for(int i=0;i<6;i++){ LedOn[i]=0; }
                    NumLedOn=0;
                }
            }
            else if(pressTime <4000)
            {
                // 2s 功能 => 改频率(闪烁模式下)
                if(CurrentMode==MODE_BLINK)
                {
                    FreqIndex=(FreqIndex+1)%3;
                    CurrentPeriod=BlinkPeriods[FreqIndex];
                }
            }
            else
            {
                // 4s => 切换闪烁/常亮
                if(CurrentMode==MODE_BLINK)
                    CurrentMode=MODE_CONSTANT;
                else
                    CurrentMode=MODE_BLINK;
            }
        }
    }
}
