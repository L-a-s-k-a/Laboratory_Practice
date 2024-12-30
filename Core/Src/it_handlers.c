#include "it_handlers.h" 

extern uint16_t DelayTickCount;  // 用来跟踪延迟计数
extern uint16_t LedCount[6];     // 控制每个LED的状态更新
extern uint32_t GlobalTickBut1Wait;  // 跟踪按钮1按下后的等待时间
extern uint8_t flagbutton;       // 用于标记按钮1是否按下。为1，表示按钮1被按下；如果为0，表示按钮1未被按下
extern uint8_t longflagbutton;   // 用于标记按钮1是否被长按。
extern uint8_t CurrentState;     // 记录当前系统的状态。
extern uint8_t LedCurrHZ;     // 表示每个LED的当前闪烁频率
extern uint8_t LedState = 0;         // 用于记录LED的状态，0: 闪烁，1: 常亮

void EXTI9_5_IRQHandler(void) // Button PC6 handler, 外部中断9到5的中断处理函数
{
    if(READ_BIT(GPIOC->IDR, GPIO_IDR_ID6) != 0) { // 按钮是否被按下，如果按钮按下，设置 flagbutton = 1 并重置 GlobalTickBut1Wait 为 0
        if(flagbutton == 0) { 
            flagbutton = 1;
            GlobalTickBut1Wait = 0;
        }
    }
    else {
        // 长按检测 2秒后改变LED频率
        if (flagbutton == 1 && GlobalTickBut1Wait >= 2000 && GlobalTickBut1Wait < 4000) {
            longflagbutton = 1; // 在2秒到4秒之间，longflagbutton 被设置为 1
            // 改变所有LED的频率
            for (uint8_t i = 0; i < 6; i++) {
                LedCurrHZ = (LedCurrHZ + 1) % 3; // 在0, 1, 2之间切换频率
            }
            flagbutton = 0;
        }
        // 长按4秒切换闪烁模式和常亮模式
        if (flagbutton == 1 && GlobalTickBut1Wait >= 4000) {
            // 切换LED的状态（闪烁 <-> 常亮）
            if (LedState == 0) {
                LedState = 1; // 切换到常亮模式
            } else {
                LedState = 0; // 切换到闪烁模式
            }
            GlobalTickBut1Wait = 0; // 重置长按计时器
            flagbutton = 0; // 清除长按标志
        }
        else if (flagbutton == 1 && GlobalTickBut1Wait >= 30) { // 按钮按下超过30毫秒（即短按），flagbutton 被清除，CurrentState 增加
            flagbutton = 0;
            CurrentState++; // 表示灯点亮的数量
        }
    }
    SET_BIT(EXTI->PR, EXTI_PR_PR6); 
}

void SysTick_Handler(void) // 中断处理函数
{  
    DelayTickCount++; // 系统定时器中断发生时，增加1
    if(flagbutton == 1) {
        GlobalTickBut1Wait++; // 按钮1处于按下状态，GlobalTickBut1Wait 每次增加1，用于跟踪按钮1的按压时间
    }

    // if(flagbut2 == 1){
    //     GlobalTickBut2Wait++;
    // }
    for (uint8_t i = 0; i < 6; i++) {
        LedCount[i]++; // 更新LED计数器
    }
} 

void mydelay(uint32_t delay) {  
    if(DelayTickCount >= delay) DelayTickCount = 0;
    while(DelayTickCount < delay) {} // Delay loop 
}  
