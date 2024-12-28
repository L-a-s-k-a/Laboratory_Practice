#include "stm32f4xx.h"
#include "init.h"
#include <stdint.h>

//-------------------------------------------------------------------
// 声明在 main.c 中定义的全局变量 (extern)
//-------------------------------------------------------------------
extern volatile uint8_t  CurrentMode;      // 1 or 2
extern volatile uint32_t GlobalTickCount;
extern volatile uint32_t ExternInterruptTickCount;

extern volatile uint8_t  M1FreqIndex;      // 0..2 => 对应模式1的三档频率
extern volatile uint8_t  M2SelectedLed;    // 模式2下当前操作的LED (0..5)
extern volatile uint8_t  LedActive[6];     // 该 LED 是否处于闪烁激活状态
extern volatile uint8_t  LedFreqIndex[6];  // 该 LED 频率档位(0..2)
extern void Mode2_SetLedActive(uint8_t ledIndex, uint8_t freqIndex);
extern void Mode2_DeactivateAll(void);     // 进入模式2时，先全灭

//-------------------------------------------------------------------
// SysTick Handler
//-------------------------------------------------------------------
void SysTick_Handler(void)
{
    // 每次进来 1ms (假设你SysTick_Init配置1ms)
    GlobalTickCount++;
    ExternInterruptTickCount++;
}

//-------------------------------------------------------------------
// EXTI15_10_IRQHandler => 这里处理 PC13 (line13)
//-------------------------------------------------------------------
void EXTI15_10_IRQHandler(void)
{
    // 处理 PC13 => line13
    if (EXTI->PR & (1 << 13))
    {
        EXTI->PR |= (1 << 13); // 清除中断标志
        if(ExternInterruptTickCount > 50) // 防抖
        {
            // 切换模式
            if(CurrentMode == 1)
            {
                CurrentMode = 2;
                Mode2_DeactivateAll(); 
            }
            else
            {
                CurrentMode = 1;
                // (可在此让模式1恢复初始状态)
            }
            ExternInterruptTickCount = 0;
        }
    }
}

//-------------------------------------------------------------------
// EXTI9_5_IRQHandler => 这里同时处理 PC6 (line6), PC7 (line7)
//-------------------------------------------------------------------
void EXTI9_5_IRQHandler(void)
{
    //=== PC8 => 按钮2 ===
    if (EXTI->PR & (1 << 8))
    {
        EXTI->PR |= (1 << 8); 
        if(ExternInterruptTickCount > 50)
        {
            if(CurrentMode == 1)
            {
                // 模式1 => 切换对灯频率
                M1FreqIndex++;
                if(M1FreqIndex >= 3) M1FreqIndex = 0;
            }
            else
            {
                // 模式2 => 切换“当前LED”频率
                if(LedActive[M2SelectedLed])
                {
                    LedFreqIndex[M2SelectedLed]++;
                    if(LedFreqIndex[M2SelectedLed] >= 3)
                        LedFreqIndex[M2SelectedLed] = 0;
                }
            }
            ExternInterruptTickCount = 0;
        }
    }

    //=== PC9 => 按钮3 ===
    if (EXTI->PR & (1 << 9))
    {
        EXTI->PR |= (1 << 9);
        if(ExternInterruptTickCount > 50)
        {
            if(CurrentMode == 1)
            {
                // 模式1 => 不做操作(或你想加别的)
            }
            else
            {
                // 模式2 => 切换当前操作的LED编号
                M2SelectedLed++;
                if(M2SelectedLed >= 6) M2SelectedLed = 0;

                // 若未激活 => 激活(默认频率0 => 0.3Hz)
                if(LedActive[M2SelectedLed] == 0)
                {
                    Mode2_SetLedActive(M2SelectedLed, 0);
                }
            }
            ExternInterruptTickCount = 0;
        }
    }
}
