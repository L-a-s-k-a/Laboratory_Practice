#include "stm32f4xx.h"   // 包含 STM32F4 系列 MCU 的寄存器定义与核心头文件
#include "init.h"       // 本项目的初始化函数声明等
#include <stdint.h>     // 标准整数类型（uint8_t、uint32_t 等）

//-------------------------------------------------------------------
// 外部引用（extern）：在 main.c 中定义的全局变量
// 这些变量在这里会被中断服务函数使用
//-------------------------------------------------------------------
extern volatile uint8_t  CurrentMode;       // 当前模式 (1=模式1, 2=模式2)
extern volatile uint32_t GlobalTickCount;   // 全局 SysTick 计数
extern volatile uint32_t ExternInterruptTickCount; 
    // 用于简单防抖或记录距离上次按键触发的时间

extern volatile uint8_t  M1FreqIndex;       // 模式1 对灯频率的档位 (0..2)
extern volatile uint8_t  M2SelectedLed;     // 模式2下当前选中的 LED (索引 0..5)
extern volatile uint8_t  LedActive[6];      // 每个 LED 是否被激活进行闪烁 (模式2)
extern volatile uint8_t  LedFreqIndex[6];   // 每个 LED 当前使用的频率档 (0..2)

// 供中断服务函数调用的外部函数
extern void Mode2_SetLedActive(uint8_t ledIndex, uint8_t freqIndex);
extern void Mode2_DeactivateAll(void);      // 进入模式2时，先将所有 LED 熄灭并取消闪烁

//-------------------------------------------------------------------
// SysTick_Handler
// 系统定时器中断，每 1ms 触发一次 (假设 SysTick_Init 配置的 1ms)
//-------------------------------------------------------------------
void SysTick_Handler(void)
{
    // 每次 SysTick 中断进来，主计数 +1
    GlobalTickCount++;
    // 防抖计数 +1，用来判断按键按下后是否已经超过某个阈值
    ExternInterruptTickCount++;
}

//-------------------------------------------------------------------
// EXTI15_10_IRQHandler
// 负责处理线号在 [10..15] 范围的外部中断，这里包括 PC13 => EXTI13
//-------------------------------------------------------------------
void EXTI15_10_IRQHandler(void)
{
    // 若是 PC13 => line13 触发
    if (EXTI->PR & (1 << 13))
    {
        // 写 1 清除中断挂起标志位
        EXTI->PR |= (1 << 13);

        // 简易防抖：若距离上次触发已超过 50ms
        if(ExternInterruptTickCount > 50)
        {
            // 切换模式：若当前是模式1 => 切到模式2；反之亦然
            if(CurrentMode == 1)
            {
                CurrentMode = 2;
                // 进入模式2 时，先关闭所有 LED 闪烁
                Mode2_DeactivateAll(); 
            }
            else
            {
                CurrentMode = 1;
                // 此处可添加代码让模式1恢复到初始状态
            }

            // 重置防抖计数器
            ExternInterruptTickCount = 0;
        }
    }
}

//-------------------------------------------------------------------
// EXTI9_5_IRQHandler
// 负责处理线号在 [5..9] 范围的外部中断，本例中用来响应 PC8 => EXTI8 (按钮2) 和 PC9 => EXTI9 (按钮3)
//-------------------------------------------------------------------
void EXTI9_5_IRQHandler(void)
{
    //=== PC8 => line8 => 按钮2 ===
    if (EXTI->PR & (1 << 8))
    {
        // 清除 line8 的中断挂起标志
        EXTI->PR |= (1 << 8);

        // 同样的防抖检查
        if(ExternInterruptTickCount > 50)
        {
            // 若当前是模式1 => 切换对灯频率
            if(CurrentMode == 1)
            {
                M1FreqIndex++;
                if(M1FreqIndex >= 3) 
                    M1FreqIndex = 0;
            }
            else
            {
                // 模式2 => 切换“当前选中 LED”的频率档
                if(LedActive[M2SelectedLed])
                {
                    LedFreqIndex[M2SelectedLed]++;
                    if(LedFreqIndex[M2SelectedLed] >= 3)
                        LedFreqIndex[M2SelectedLed] = 0;
                }
            }
            // 重置防抖计数器
            ExternInterruptTickCount = 0;
        }
    }

    //=== PC9 => line9 => 按钮3 ===
    if (EXTI->PR & (1 << 9))
    {
        // 清除 line9 的中断挂起标志
        EXTI->PR |= (1 << 9);

        // 防抖判断
        if(ExternInterruptTickCount > 50)
        {
            // 如果当前在模式1，则按钮3不执行任何操作 (或可自定义其它功能)
            if(CurrentMode == 1)
            {
                // no operation
            }
            else
            {
                // 模式2 => 切换“当前操作 LED”编号 (0..5)
                M2SelectedLed++;
                if(M2SelectedLed >= 6) 
                    M2SelectedLed = 0;

                // 如果该 LED 未激活，则激活它，并默认频率为 0 (对应 0.3Hz)
                if(LedActive[M2SelectedLed] == 0)
                {
                    Mode2_SetLedActive(M2SelectedLed, 0);
                }
            }
            // 防抖计数器归零
            ExternInterruptTickCount = 0;
        }
    }
}