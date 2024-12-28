#include "it_handlers.h"

// 参考示例中的全局变量命名方式
extern volatile uint32_t g_systickCount; // 1ms全局计数
// 下文我们在 main.c 里定义

static uint32_t btn1_lastPress = 0;  // PC13
static uint32_t btn2_lastPress = 0;  // PC6
static uint32_t btn3_lastPress = 0;  // PC7

void SysTick_Handler(void)
{
    g_systickCount++; 
}

// -------------------- PC13 => EXTI15_10_IRQHandler --------------------
void EXTI15_10_IRQHandler(void)
{
    // 检查是否是 line13 触发
    if (READ_BIT(EXTI->PR, EXTI_PR_PR13) != 0)
    {
        // 清除中断标志
        SET_BIT(EXTI->PR, EXTI_PR_PR13);

        // 消抖判断
        if ( (g_systickCount - btn1_lastPress) >= DELAY_BUTTON_FILTER )
        {
            btn1_lastPress = g_systickCount;
            // 按键1功能：切换模式1 <-> 模式2
            extern volatile uint8_t g_workMode;
            g_workMode = (g_workMode == 1) ? 2 : 1;

            // 如果刚切到模式2 => 熄灭全部LED
            // 如果刚切到模式1 => 可以考虑恢复对灯逻辑(但通常自动进行)
            if (g_workMode == 2)
            {
                // 关灯
                GPIOB->BSRR = ( LED1_PIN|LED2_PIN|LED3_PIN|
                                LED4_PIN|LED5_PIN|LED6_PIN ) << 16;
            }
        }
    }
}

// -------------------- PC6, PC7 => EXTI9_5_IRQHandler --------------------
void EXTI9_5_IRQHandler(void)
{
    // 先检测 PC6 => EXTI6 (按键2)
    if (READ_BIT(EXTI->PR, EXTI_PR_PR6) != 0)
    {
        SET_BIT(EXTI->PR, EXTI_PR_PR6);

        if ( (g_systickCount - btn2_lastPress) >= DELAY_BUTTON_FILTER )
        {
            btn2_lastPress = g_systickCount;
            
            extern volatile uint8_t g_workMode; 
            if (g_workMode == 1)
            {
                // 模式1 => 切换 对灯 频率
                extern volatile uint8_t g_mode1FreqIndex; 
                g_mode1FreqIndex = (g_mode1FreqIndex + 1) % 3; 
            }
            else
            {
                // 模式2 => 切换“当前LED”的频率
                extern volatile uint8_t g_currentLedIndex[6];
                extern volatile uint8_t g_selectedLed; 
                // 对 g_selectedLed 对应的 freqIndex 自增
                // 如果原先 -1 表示未激活，也可以先令其=0再切换；
                // 这里简化：假设一旦被选中过，就必有 freqIndex>=0
                g_currentLedIndex[g_selectedLed] 
                    = (g_currentLedIndex[g_selectedLed] + 1) % 3;
            }
        }
    }

    // 再检测 PC7 => EXTI7 (按键3)
    if (READ_BIT(EXTI->PR, EXTI_PR_PR7) != 0)
    {
        SET_BIT(EXTI->PR, EXTI_PR_PR7);

        if ( (g_systickCount - btn3_lastPress) >= DELAY_BUTTON_FILTER )
        {
            btn3_lastPress = g_systickCount;

            extern volatile uint8_t g_workMode; 
            if (g_workMode == 2)
            {
                // 模式2 => 选择下一个LED
                extern volatile uint8_t g_selectedLed;
                extern volatile uint8_t g_currentLedIndex[6];

                g_selectedLed = (g_selectedLed + 1) % 6;
                // 若该LED还没被激活过(可用-1表示)，则设为 freqIndex=0(0.3Hz)
                if (g_currentLedIndex[g_selectedLed] > 2)
                {
                    g_currentLedIndex[g_selectedLed] = 0;
                }
            }
            else
            {
                // 模式1 => 按键3不做功能
            }
        }
    }
}