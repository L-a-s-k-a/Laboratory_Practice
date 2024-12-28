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
    // 判断是不是 line13 触发
    if((EXTI->PR & (1 << 13)) != 0)
    {
        // 清中断挂起标志
        EXTI->PR |= (1 << 13);

        // 防抖
        if(ExternInterruptTickCount > 50) // 设个 50ms 的简单防抖
        {
            // 切换模式
            if(CurrentMode == 1)
            {
                // 切到模式2
                CurrentMode = 2;
                // 进入模式2时，先全灭
                Mode2_DeactivateAll();
            }
            else
            {
                // 切到模式1
                CurrentMode = 1;
                // （可选）切回模式1时，立即让对灯重新开始
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
    //==== 处理 PC6 (line6) => 按键2 ====
    if((EXTI->PR & (1 << 6)) != 0)
    {
        EXTI->PR |= (1 << 6); // 清中断标志

        if(ExternInterruptTickCount > 50) // 简单防抖
        {
            if(CurrentMode == 1)
            {
                // 模式1: 切换对灯频率档 (0.5Hz / 1.7Hz / 2.2Hz)
                M1FreqIndex++;
                if(M1FreqIndex >= 3) M1FreqIndex = 0;
            }
            else
            {
                // 模式2: 切换 “当前 LED” 的频率
                if(LedActive[M2SelectedLed] == 1)
                {
                    // 该LED已激活，修改其档位
                    LedFreqIndex[M2SelectedLed]++;
                    if(LedFreqIndex[M2SelectedLed] >= 3) 
                        LedFreqIndex[M2SelectedLed] = 0;
                }
                else
                {
                    // 如果该 LED 尚未激活，理论上按键2 不做事；
                    // 或你想按键2 一按就激活? 这要看需求定义。
                    // 此处暂不做激活动作。
                }
            }

            ExternInterruptTickCount = 0;
        }
    }

    //==== 处理 PC7 (line7) => 按键3 ====
    if((EXTI->PR & (1 << 7)) != 0)
    {
        EXTI->PR |= (1 << 7); // 清中断标志

        if(ExternInterruptTickCount > 50)
        {
            if(CurrentMode == 1)
            {
                // 模式1: 不做任何事情
            }
            else
            {
                // 模式2: 切换“当前操作LED” (0..5)
                M2SelectedLed++;
                if(M2SelectedLed >= 6) 
                    M2SelectedLed = 0;

                // 若该 LED 还未激活，则给它默认 0.3Hz 并启动闪烁
                if(LedActive[M2SelectedLed] == 0)
                {
                    Mode2_SetLedActive(M2SelectedLed, 0); 
                    // freqIndex=0 => 0.3Hz
                }
            }

            ExternInterruptTickCount = 0;
        }
    }
}