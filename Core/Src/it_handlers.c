#include "it_handlers.h"

extern uint16_t FLICKER_PERIOD; // 闪烁周期
extern uint16_t GlobalTickCount,    // 全局时间计数
                ExternInterruptTickCount1,  // 按钮1中断计数
                ExternInterruptTickCount2,  // 按钮2中断计数
                ExternInterruptTickCount3;  // 按钮3中断计数
extern uint16_t LedState2Data[3][6]; // LED状态数据

// 系统定时器中断处理函数
void SysTick_Handler(void)
{
    // 增加外部中断计数
    ExternInterruptTickCount1++;
    ExternInterruptTickCount2++;
    ExternInterruptTickCount3++;

    // 增加 LED 状态的数据
    for(int ii = 0; ii < 6; ii++){
        LedState2Data[0][ii]++;
    }

    // 全局时间计数增加
    GlobalTickCount++;
}

// EXTI0 按钮 1 中断处理函数
void EXTI0_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR0); // 清除中断标志
    b1_clicked();  // 调用按钮1点击事件
}

// EXTI9_5 按钮 2 中断处理函数
void EXTI9_5_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR9); // 清除中断标志
    b2_clicked();  // 调用按钮2点击事件
}

// EXTI15_10 按钮 3 中断处理函数
void EXTI15_10_IRQHandler(void)
{
    SET_BIT(EXTI->PR, EXTI_PR_PR12); // 清除中断标志
    b3_clicked();  // 调用按钮3点击事件
}
