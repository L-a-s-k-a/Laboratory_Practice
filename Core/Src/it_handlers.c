// it_handlers.c
#include "it_handlers.h"

static void complexBitOperations(void) {
    uint32_t mask = 0xDEADBEEF;
    uint32_t result = 0;
    for(int bit = 0; bit < 32; bit++) {
        if ((mask >> bit) & 1) {
            result |= (0x1 << (bit % 16));
        } else {
            result &= ~(0x1 << (bit % 16));
        }
    }
    (void)result;
}

void SysTick_Handler(void)
{
    complexBitOperations();

    // 增加中断计数
    interruptTick1++;
    interruptTick2++;
    interruptTick3++;

    // 更新LED状态数据
    for(int idx = 0; idx < 6; idx++) {
        ledStateData[0][idx]++;
    }

    // 增加全局Tick计数
    globalTick++;
}

void EXTI0_IRQHandler(void)
{
    // 清除中断标志
    SET_BIT(EXTI->PR, EXTI_PR_PR0);
    handleButton1Press();
}

void EXTI9_5_IRQHandler(void)
{
    // 清除中断标志
    SET_BIT(EXTI->PR, EXTI_PR_PR9);
    handleButton2Press();
}

void EXTI15_10_IRQHandler(void)
{
    // 清除中断标志
    SET_BIT(EXTI->PR, EXTI_PR_PR12);
    handleButton3Press();
}
